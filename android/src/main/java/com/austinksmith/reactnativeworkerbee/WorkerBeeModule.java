package com.austinksmith.reactnativeworkerbee;

import android.util.Log;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.module.annotations.ReactModule;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;
import com.facebook.react.turbomodule.core.interfaces.CallInvokerHolder;
import com.facebook.jni.HybridData;

@ReactModule(name = WorkerBeeModule.NAME)
public class WorkerBeeModule extends ReactContextBaseJavaModule {
    public static final String NAME = "WorkerBeeModule";

    static {
        System.loadLibrary("workerbee-jni");
    }

    private final HybridData mHybridData;

    public WorkerBeeModule(ReactApplicationContext reactContext) {
        super(reactContext);
        mHybridData = initHybrid(reactContext.getJavaScriptContextHolder().get(),
                                 (CallInvokerHolderImpl) reactContext.getCatalystInstance().getJSCallInvokerHolder());
    }

    @Override
    public String getName() {
        return NAME;
    }

    private native HybridData initHybrid(long jsContext, CallInvokerHolder jsCallInvokerHolder);
    private native void installJSIBindings();

    @ReactMethod(isBlockingSynchronousMethod = true)
    public boolean install() {
        try {
            installJSIBindings();
            return true;
        } catch (Exception e) {
            Log.e(NAME, "Failed to install JSI bindings", e);
            return false;
        }
    }

    public native long createWorker(String scriptURL);
    public native void postMessage(long workerId, String message);
    public native void terminateWorker(long workerId);
}