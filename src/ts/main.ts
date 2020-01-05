import { DrivingWheelTurnEventName, SteeringWheel } from 'simple-steering-wheel';
// import { debounce } from './utils';
const wheel: SteeringWheel = document.querySelector('#servo');
const whileAngle = document.getElementById('wheel-angle');
const debugMonitor = document.querySelector('.debug-monitor') as HTMLElement;

const normalTime = 100; // ms
let nextTaskData: { [key in Props]: string };
let inTask = false;
enum Props {
  angle = 'driveWheelAngle',
  power = 'drivePower'
}

type RequestData = { [key in Props]?: string };

function initDebugLineSwitcher() {
  const switcher = document.querySelector('.monitor-switcher-placeholder input') as HTMLInputElement;
  switcher.addEventListener('change', () => {
    debugMonitor.hidden = !switcher.checked;
  });
}
initDebugLineSwitcher();
function addDebugLine(value, className = '') {
  const line = document.createElement('div');
  line.innerText = value;
  line.className = className;
  debugMonitor.appendChild(line);
  debugMonitor.scrollTop = 100000000;
}

function sendRequest(requestData: RequestData) {
  const time = Date.now();
  inTask = true;
  const url = new URL('/wheels', location.origin);
  url.search = new URLSearchParams(requestData).toString();

  const request = new XMLHttpRequest();
  request.timeout = 100;
  request.addEventListener('loadstart', () => {
    setTimeout(() => {
      request.abort();
      const interval = Date.now() - time;
      addDebugLine(`HANGED ${JSON.stringify(requestData)}, ${request.responseText}, ${interval}ms`, 'danger');
    }, 50);
  });
  request.addEventListener('load', () => {
    const interval = Date.now() - time;
    addDebugLine(
      `${JSON.stringify(requestData)}, ${request.responseText}, ${interval}ms`,
      interval > normalTime ? 'danger' : ''
    );
  });
  request.addEventListener('loadend', () => {
    inTask = false;
    if (nextTaskData) {
      sendRequest(nextTaskData);
      nextTaskData = null;
    }
  });

  request.open('PUT', url.href);
  request.send();
}
function sendRequestSubsequently(data: RequestData) {
  if (inTask) {
    nextTaskData = Object.assign({}, nextTaskData, data);
  } else {
    sendRequest(data);
  }
}

function handler() {
  const value = wheel.value.toString();
  sendRequestSubsequently({ [Props.angle]: value });
  whileAngle.innerText = value;
}
wheel.addEventListener(DrivingWheelTurnEventName, handler);

const speedControl = document.getElementById('power') as SteeringWheel;
const speedValue = document.getElementById('speed-control-value');
let curPower = 0;
speedControl.addEventListener(DrivingWheelTurnEventName, e => {
  const newPower = Math.abs(speedControl.value) < 10 ? 0 : speedControl.value;
  if (newPower === curPower) {
    return;
  }
  curPower = newPower;
  const effectivePower = curPower ? Math.sign(curPower) * (Math.abs(curPower / 2) + 55) : 0;

  sendRequestSubsequently({ [Props.power]: effectivePower.toString() });
  speedValue.innerText = speedControl.value.toString();
});
