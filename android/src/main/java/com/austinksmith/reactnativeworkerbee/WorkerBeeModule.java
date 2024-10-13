package com.austinksmith.reactnativeworkerbee;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.module.annotations.ReactModule;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;
import com.facebook.react.turbomodule.core.interfaces.CallInvokerHolder;
import com.facebook.jni.HybridData;

@ReactModule(name = WorkerBeeModule.NAME) // Annotation that makes this module available to React Native
public class WorkerBeeModule extends ReactContextBaseJavaModule {
    public static final String NAME = "WorkerBeeModule"; // The name of the module

    private final HybridData mHybridData; // To store hybrid data

    public WorkerBeeModule(ReactApplicationContext reactContext) {
        super(reactContext);
        // Initialize HybridData using the JavaScript context and the call invoker holder
        mHybridData = initHybrid(reactContext.getJavaScriptContextHolder().get(),
                                  (CallInvokerHolderImpl) reactContext.getCatalystInstance().getJSCallInvokerHolder());
    }

    @Override
    public String getName() {
        return NAME; // Return the name of the module
    }

    // Declare native methods to be implemented in C++
    private native HybridData initHybrid(long jsContext, CallInvokerHolder jsCallInvokerHolder);
    private native void installJSIBindings();

    @ReactMethod(isBlockingSynchronousMethod = true)
    public boolean install() {
        installJSIBindings(); // Call the native method to install JSI bindings
        return true; // Return true to indicate success
    }

    public native long createWorker(String scriptURL); // Native method to create a worker
    public native void postMessage(long workerId, String message); // Method to post a message to the worker
    public native void terminateWorker(long workerId); // Method to terminate the worker
}
