// import { DrivingWheelTurn, DrivingWheelTurnEnd, DrivingWheelTurnStart, SteeringWheel } from 'simple-steering-wheel';
import 'simple-steering-wheel';
import { AreaSelector } from 'simple-steering-wheel';
import { Props, sendRequestSubsequently } from './connection.service';

// import { debounce } from './utils';
// const wheel: SteeringWheel = document.querySelector('#servo');
// const whileAngle = document.getElementById('wheel-angle');
// const debugMonitor = document.querySelector('.debug-monitor') as HTMLElement;

// const maxAngle = 50;
// const touched = 'touched';

function initImageSwitcher() {
  const img = document.createElement('img');
  const src = `http://${location.hostname}:81/stream`;
  const placeholder = document.querySelector('.img-placeholder');
  const switcher = document.querySelector('.monitor-switcher-placeholder .video') as HTMLInputElement;
  switcher.addEventListener('change', () => {
    if (switcher.checked) {
      img.src = src;
      placeholder.appendChild(img);
    } else {
      placeholder.removeChild(img);
      img.src = null;
    }
  });
}

function initLed() {
  const led = document.querySelector('.monitor-switcher-placeholder .light') as HTMLInputElement;
  led.addEventListener('change', () => {
    sendRequestSubsequently({ [Props.Led]: led.checked.toString() });
  });
}

// function addDebugLine(value, className = '') {
//   const line = document.createElement('div');
//   line.innerText = value;
//   line.className = className;
//   debugMonitor.appendChild(line);
//   debugMonitor.scrollTop = 100000000;
// }

// function initPower() {
//   const speedControl = document.getElementById('power') as SteeringWheel;
//   const speedValue = document.getElementById('speed-control-value');
//   const wrapper = document.querySelector('.speed-control-wrapper');
//   let curPower = 0;
//   speedControl.addEventListener(DrivingWheelTurn, e => {
//     const value = speedControl.value;
//     const newPower = Math.abs(value) < 10 ? 0 : value;
//     if (newPower === curPower) {
//       return;
//     }
//     curPower = newPower;
//     const effectivePower = curPower ? Math.sign(curPower) * (Math.abs(curPower / 2) + 55) : 0;

//     sendRequestSubsequently({ [Props.Power]: effectivePower.toString() });
//     speedValue.innerText = value.toString();
//     wrapper.classList.toggle('max-power', Math.abs(value) === 90);
//   });
//   speedControl.addEventListener(DrivingWheelTurnStart, () => wrapper.classList.add(touched));
//   speedControl.addEventListener(DrivingWheelTurnEnd, () => wrapper.classList.remove(touched));
// }

// function initSpeed() {
//   const wrapper = document.querySelector('.wheel');

//   function handler() {
//     const value = wheel.value.toString();
//     sendRequestSubsequently({ [Props.Angle]: value });
//     whileAngle.innerText = value;
//     wrapper.classList.toggle('left', wheel.value < -4);
//     wrapper.classList.toggle('right', wheel.value > 4);
//     wrapper.classList.toggle('left-max', wheel.value === -maxAngle);
//     wrapper.classList.toggle('right-max', wheel.value === maxAngle);
//   }
//   wheel.addEventListener(DrivingWheelTurn, handler);
//   wheel.addEventListener(DrivingWheelTurnStart, () => wheel.classList.add(touched));
//   wheel.addEventListener(DrivingWheelTurnEnd, () => wheel.classList.remove(touched));
// }

function initArea() {
  const area = document.querySelector('area-selector') as AreaSelector;
  const maxWheelTurn = 50;
  const minPower = 55;
  const maxPower = 100;
  area.addEventListener('changed', () => {
    const position = area.pointFromZeroPercents();
    const newPower = Math.sign(position.y) * Math.round(Math.abs(position.y) * (maxPower - minPower) + minPower);
    const newAngle = Math.round(position.x * maxWheelTurn);
    sendRequestSubsequently({ [Props.Power]: newPower.toString(), [Props.Angle]: newAngle.toString() });
  });
}

function initHandlers() {
  // initSpeed();
  // initPower();
  initLed();
  initImageSwitcher();
  initArea();
}

initHandlers();
