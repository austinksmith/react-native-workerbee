declare module 'react-native-workerbee' {
  export default class WorkerBee {
    constructor(scriptURL: string);
    postMessage(message: any): void;
    terminate(): void;
  }
}