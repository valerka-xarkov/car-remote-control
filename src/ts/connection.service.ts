export type RequestData = { [key in Props]?: string };

// const normalTime = 100; // ms
const minTime = 30; // ms

export enum Props {
  Angle = 'driveWheelAngle',
  Power = 'drivePower',
  Led = 'led'
}

let nextTaskData: { [key in Props]: string };
let inTask = false;

function sendRequest(requestData: RequestData) {
  const time = Date.now();
  inTask = true;

  const request = new XMLHttpRequest();
  request.addEventListener('load', () => {
    // const interval = Date.now() - time;
    // addDebugLine(
    //   `${JSON.stringify(requestData)}, ${request.responseText}, ${interval}ms`,
    //   interval > normalTime ? 'danger' : ''
    // );
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

  request.open(
    'GET',
    '/wheels?' +
      Object.keys(requestData)
        .map(k => `${k}=${requestData[k]}`)
        .join('&')
  );
  request.send();
}

export function sendRequestSubsequently(data: RequestData) {
  if (inTask) {
    nextTaskData = Object.assign({}, nextTaskData, data);
  } else {
    sendRequest(data);
  }
}
