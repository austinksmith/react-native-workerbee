#include <jni.h>
#include <jsi/jsi.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <fbjni/fbjni.h>
#include <thread>
#include <unordered_map>

using namespace facebook::jsi;
using namespace facebook::jni;
using namespace std;

class WorkerBeeJSI : public jni::HybridClass<WorkerBeeJSI> {
public:
    static constexpr auto kJavaDescriptor = "Lcom/austinksmith/reactnativeworkerbee/WorkerBeeModule;";

    static void registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", WorkerBeeJSI::initHybrid),
            makeNativeMethod("installJSIBindings", WorkerBeeJSI::installJSIBindings),
            makeNativeMethod("createWorker", WorkerBeeJSI::createWorker),
            makeNativeMethod("postMessage", WorkerBeeJSI::postMessage),
            makeNativeMethod("terminateWorker", WorkerBeeJSI::terminateWorker),
        });
    }

private:
    friend HybridBase;
    jni::global_ref<jobject> javaPart_;
    Runtime* runtime_;
    std::shared_ptr<CallInvoker> jsCallInvoker_;
    std::unordered_map<long, std::thread> workers_;
    long nextWorkerId_ = 1;

    explicit WorkerBeeJSI(jni::alias_ref<jhybridobject> jThis)
        : javaPart_(jni::make_global(jThis)) {}

    static void installJSIBindings(jni::alias_ref<jhybridobject> jThis) {
        auto instance = getInstance(jThis);
        instance->installJSIBindingsInternal();
    }

    void installJSIBindingsInternal() {
        auto workerObject = Object(*runtime_);
        auto createWorkerFunc = Function::createFromHostFunction(*runtime_,
            PropNameID::forAscii(*runtime_, "createWorker"),
            1,
            [this](Runtime& runtime, const Value& thisValue, const Value* arguments, size_t count) -> Value {
                string scriptURL = arguments[0].getString(runtime).utf8(runtime);
                return Value(createWorkerInternal(scriptURL));
            });
        workerObject.setProperty(*runtime_, "createWorker", createWorkerFunc);

        // TODO: Add similar functions for postMessage and terminateWorker

        runtime_->global().setProperty(*runtime_, "WorkerBeeModule", workerObject);
    }

    long createWorkerInternal(const string& scriptURL) {
        long workerId = nextWorkerId_++;
        workers_[workerId] = std::thread([this, workerId, scriptURL]() {
            // TODO: Implement worker thread logic
        });
        return workerId;
    }

    // TODO: Implement postMessage and terminateWorker methods

public:
    static jni::local_ref<jhybriddata> initHybrid(
        jni::alias_ref<jhybridobject> jThis,
        jlong jsContext,
        jni::alias_ref<CallInvokerHolder::javaobject> jsCallInvokerHolder) {
        auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
        return makeCxxInstance(jThis, reinterpret_cast<Runtime*>(jsContext), jsCallInvoker);
    }
};

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*) {
    return jni::initialize(vm, [] {
        WorkerBeeJSI::registerNatives();
    });
}