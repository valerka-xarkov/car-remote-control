import { DrivingWheelTurn, DrivingWheelTurnEnd, DrivingWheelTurnStart, SteeringWheel } from 'simple-steering-wheel';
// import { debounce } from './utils';
const wheel: SteeringWheel = document.querySelector('#servo');
const whileAngle = document.getElementById('wheel-angle');
const debugMonitor = document.querySelector('.debug-monitor') as HTMLElement;

const normalTime = 100; // ms
const minTime = 30; // ms
const touched = 'touched';
let nextTaskData: { [key in Props]: string };
let inTask = false;
enum Props {
  Angle = 'driveWheelAngle',
  Power = 'drivePower'
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

  const request = new XMLHttpRequest();
  request.addEventListener('load', () => {
    const interval = Date.now() - time;
    addDebugLine(
      `${JSON.stringify(requestData)}, ${request.responseText}, ${interval}ms`,
      interval > normalTime ? 'danger' : ''
    );
  });
  request.addEventListener('loadend', () => {
    const interval = Date.now() - time;
    setTimeout(() => {
      inTask = false;
      if (nextTaskData) {
        sendRequest(nextTaskData);
        nextTaskData = null;
      }
    }, Math.max(0, minTime - interval));
  });

  request.open('PUT', '/wheels');
  request.send(
    Object.keys(requestData)
      .map(k => `${k}=${requestData[k]}`)
      .join('&')
  );
}
function sendRequestSubsequently(data: RequestData) {
  if (inTask) {
    nextTaskData = Object.assign({}, nextTaskData, data);
  } else {
    sendRequest(data);
  }
}

function initPower() {
  const speedControl = document.getElementById('power') as SteeringWheel;
  const speedValue = document.getElementById('speed-control-value');
  const wrapper = document.querySelector('.speed-control-wrapper');
  let curPower = 0;
  speedControl.addEventListener(DrivingWheelTurn, e => {
    const newPower = Math.abs(speedControl.value) < 10 ? 0 : speedControl.value;
    if (newPower === curPower) {
      return;
    }
    curPower = newPower;
    const effectivePower = curPower ? Math.sign(curPower) * (Math.abs(curPower / 2) + 55) : 0;

    sendRequestSubsequently({ [Props.Power]: effectivePower.toString() });
    speedValue.innerText = speedControl.value.toString();
  });
  speedControl.addEventListener(DrivingWheelTurnStart, () => wrapper.classList.add(touched));
  speedControl.addEventListener(DrivingWheelTurnEnd, () => wrapper.classList.remove(touched));
}

function initSpeed() {
  const wrapper = document.querySelector('.wheel');

  function handler() {
    const value = wheel.value.toString();
    sendRequestSubsequently({ [Props.Angle]: value });
    whileAngle.innerText = value;
    wrapper.classList.toggle('left', wheel.value < 0);
    wrapper.classList.toggle('right', wheel.value > 0);
  }
  wheel.addEventListener(DrivingWheelTurn, handler);
  wheel.addEventListener(DrivingWheelTurnStart, () => wheel.classList.add(touched));
  wheel.addEventListener(DrivingWheelTurnEnd, () => wheel.classList.remove(touched));
}
function initHandlers() {
  initSpeed();
  initPower();
}

initHandlers();
