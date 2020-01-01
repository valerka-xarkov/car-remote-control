import { DrivingWheelTurnEventName, SteeringWheel } from 'simple-steering-wheel';
// import { debounce } from './utils';
const wheel: SteeringWheel = document.querySelector('#servo');
const whileAngle = document.getElementById('wheel-angle');
const debugMonitor = document.querySelector('.debug-monitor');

const normalTime = 100; // ms
let nextTaskData: {[key in Props]: string};
let inTask = false;
enum Props {
  angle = 'driveWheelAngle',
  power = 'drivePower'
}

type RequestData = {[key in Props]?: string};

function showDebugLine(value, className = '') {
  const line = document.createElement('div');
  line.innerText = value;
  line.className = className;
  debugMonitor.appendChild(line);
  debugMonitor.scrollTop = 100000000;
}
function sendRequest(request: RequestData) {
  const time = Date.now();
  inTask = true;
  console.log(new Date().toISOString())

  const url = new URL('/wheels', location.origin);
  // url.search = new URLSearchParams({ driveWheelAngle: curSteering.toString(), drivePower: effectivePower.toString() }).toString()
  url.search = new URLSearchParams(request).toString()

  fetch(url.href, {
    cache: 'no-cache',
    method: 'PUT',
  })
  .then(r => r.text())
  .then(message => {
    const interval = Date.now() - time;
    showDebugLine(`${JSON.stringify(request)}, ${message}, ${interval}ms`, interval > normalTime ? 'danger': '')
  })
  // .then(() => new Promise(r => setTimeout(() => r(), 1000)))
  .finally(() => {
    inTask = false;
    if (nextTaskData) {
      sendRequest(nextTaskData)
      nextTaskData = null;
    }
  });
}
function sendRequestSubsiquentelly(data: RequestData) {
  if (inTask) {
    nextTaskData = Object.assign({}, nextTaskData, data);
  } else {
    sendRequest(data);
  }
}

function handler() {
  const value = wheel.value.toString();
  sendRequestSubsiquentelly({[Props.angle]: value});
  whileAngle.innerText = value;
}
wheel.addEventListener(DrivingWheelTurnEventName, handler);

const speedControl = document.getElementById('speed-control') as HTMLInputElement;
const speedValue = document.getElementById('speed-control-value');
speedControl.addEventListener('input', e => {
  e.stopPropagation();
  const curPower = +speedControl.value;
  const effectivePower = curPower ? Math.sign(curPower) * (Math.abs(curPower) + 60) : 0;

  sendRequestSubsiquentelly({[Props.power]: effectivePower.toString()});
  speedValue.innerText = speedControl.value;
});
