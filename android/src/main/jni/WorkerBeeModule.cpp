#include <jni.h>
#include <jsi/jsi.h>
#include <fbjni/fbjni.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <thread>
#include <unordered_map>
#include <iostream>

using namespace facebook::jsi;
using namespace facebook::jni;
using namespace std;

class WorkerBeeModule : public facebook::jni::HybridClass<WorkerBeeModule> {
public:
    static auto constexpr kJavaDescriptor = "Lcom/austinksmith/reactnativeworkerbee/WorkerBeeModule;";

    static void registerNatives() {
        javaClassStatic()->registerNatives({
            makeNativeMethod("initHybrid", WorkerBeeModule::initHybrid),
            makeNativeMethod("installJSIBindings", WorkerBeeModule::installJSIBindings),
            makeNativeMethod("createWorker", WorkerBeeModule::createWorker),
            makeNativeMethod("postMessage", WorkerBeeModule::postMessage),
            makeNativeMethod("terminateWorker", WorkerBeeModule::terminateWorker)
        });
    }

    static facebook::jni::local_ref<jhybriddata> initHybrid(
        facebook::jni::alias_ref<jhybridobject> jThis,
        jlong jsContext,
        facebook::jni::alias_ref<facebook::react::CallInvokerHolder::javaobject> jsCallInvokerHolder) {
        
        auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
        return makeCxxInstance(jThis, reinterpret_cast<Runtime*>(jsContext), jsCallInvoker);
    }

private:
    friend HybridClass;
    
    facebook::jni::global_ref<jobject> javaPart_;
    Runtime* runtime_;
    std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker_;
    std::unordered_map<long, std::thread> workers_;
    long nextWorkerId_ = 1;

    explicit WorkerBeeModule(
        facebook::jni::alias_ref<jhybridobject> jThis,
        Runtime* runtime,
        std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker)
        : javaPart_(make_global(jThis))
        , runtime_(runtime)
        , jsCallInvoker_(jsCallInvoker) {}

    void installJSIBindings() {
        auto workerObject = Object(*runtime_);
        auto createWorkerFunc = Function::createFromHostFunction(
            *runtime_,
            PropNameID::forAscii(*runtime_, "createWorker"),
            1,
            [this](Runtime& runtime, const Value& thisValue, const Value* arguments, size_t count) -> Value {
                string scriptURL = arguments[0].getString(runtime).utf8(runtime);
                return Value((double)createWorkerInternal(scriptURL));
            });

        workerObject.setProperty(*runtime_, "createWorker", std::move(createWorkerFunc));
        runtime_->global().setProperty(*runtime_, "WorkerBeeModule", std::move(workerObject));
    }

    long createWorkerInternal(const string& scriptURL) {
        long workerId = nextWorkerId_++;
        workers_[workerId] = std::thread([this, workerId, scriptURL]() {
            cout << "Worker " << workerId << " started with script: " << scriptURL << endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            cout << "Worker " << workerId << " finished." << endl;
        });
        return workerId;
    }

    void createWorker(jstring scriptURL) {
        auto nativeString = make_local(scriptURL)->toStdString();
        createWorkerInternal(nativeString);
    }

    void postMessage(jlong workerId, jstring message) {
        if (workers_.find(workerId) != workers_.end()) {
            auto nativeMessage = make_local(message)->toStdString();
            cout << "Message to worker " << workerId << ": " << nativeMessage << endl;
        }
    }

    void terminateWorker(jlong workerId) {
        if (workers_.find(workerId) != workers_.end()) {
            if (workers_[workerId].joinable()) {
                workers_[workerId].join();
            }
            workers_.erase(workerId);
            cout << "Worker " << workerId << " terminated." << endl;
        }
    }
};

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*) {
    return facebook::jni::initialize(vm, [] {
        WorkerBeeModule::registerNatives();
    });
}