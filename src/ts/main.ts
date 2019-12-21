import { DrivingWheelTurnEventName, SteeringWheel } from 'simple-steering-wheel';
import { debounce } from './utils';
const wheel: SteeringWheel = document.querySelector('#servo');
const whileAngle = document.getElementById('wheel-angle');

let steering = 0;
let power = 0;
function sendRequest() {
  fetch('/wheels', {
    body: JSON.stringify({ driveWheelAngle: steering, drivePower: power }),
    cache: 'no-cache',
    headers: {
      'Content-Type': 'application/json'
    },
    method: 'PUT',
  });
}
const debouncedSendRequest = debounce(sendRequest, 50, 100);

function handler() {
  steering = wheel.value;
  debouncedSendRequest();
  whileAngle.innerText = wheel.value.toString();
}
wheel.addEventListener(DrivingWheelTurnEventName, handler);

const speedControl = document.getElementById('speed-control') as HTMLInputElement;
const speedValue = document.getElementById('speed-control-value');
speedControl.addEventListener('input', e => {
  power = + speedControl.value;
  e.stopPropagation();
  debouncedSendRequest();
  speedValue.innerText = speedControl.value;
});
