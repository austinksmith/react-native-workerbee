# react-native-workerbee

A Web Worker-like implementation for React Native using JSI.

**Author**: Austin K. Smith

![react-native-workerbee-logo](https://asmithdev.com/img/workerbee.png)


[![npm version](https://img.shields.io/npm/v/react-native-workerbee.svg?style=flat-square)](https://www.npmjs.com/package/react-native-workerbee)
[![downloads](https://img.shields.io/npm/dm/react-native-workerbee.svg?style=flat-square)](https://www.npmjs.com/package/react-native-workerbee)

## Installation

```sh
npm install react-native-workerbee
# or
yarn add react-native-workerbee
```

### Android Setup

1. Open up `android/app/src/main/java/[...]/MainApplication.java`
   - Add `import com.austinksmith.reactnativeworkerbee.WorkerBeePackage;` to the imports at the top of the file
   - Add `packages.add(new WorkerBeePackage());` in the `getPackages()` method

2. Append the following lines to `android/settings.gradle`:
   ```gradle
   include ':react-native-workerbee'
   project(':react-native-workerbee').projectDir = new File(rootProject.projectDir, '../node_modules/react-native-workerbee/android')
   ```

3. Insert the following lines inside the dependencies block in `android/app/build.gradle`:
   ```gradle
   implementation project(':react-native-workerbee')
   ```

## Usage

```javascript
import WorkerBee from 'react-native-workerbee';

// Create a new worker
const worker = new WorkerBee('path/to/your/worker/script.js');

// Post a message to the worker
worker.postMessage({ type: 'START', payload: { /* ... */ } });

// Terminate the worker
worker.terminate();
```

## API

### `constructor(scriptURL: string)`

Creates a new WorkerBee instance.

- `scriptURL`: The path to your worker script file.

### `postMessage(message: any)`

Sends a message to the worker.

- `message`: The message to send to the worker. This will be serialized to JSON.

### `terminate()`

Terminates the worker.

## Platform Support

Currently, this module only supports Android. iOS support is planned for future releases.

## License

Artistic License 2.0