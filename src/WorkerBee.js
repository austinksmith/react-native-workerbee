import { NativeModules, Platform } from 'react-native';

const { WorkerBeeModule } = NativeModules;

class WorkerBee {
  constructor(scriptURL) {
    if (Platform.OS !== 'android') {
      throw new Error('WorkerBee is currently only supported on Android');
    }
    this.workerId = WorkerBeeModule.createWorker(scriptURL);
  }

  postMessage(message) {
    WorkerBeeModule.postMessage(this.workerId, JSON.stringify(message));
  }

  terminate() {
    WorkerBeeModule.terminateWorker(this.workerId);
  }
}

export default WorkerBee;