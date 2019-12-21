// declare var ResizeObserver: any;

// export class AriaSelectionEvent extends Event {
//   constructor(public x: number, public y: number) {
//     super('aria-selection');
//   }
// }

// export class AriaSelector extends HTMLElement {
//   static readonly observedAttributes = ['ps'];
//   private size = 10;
//   private pointer = document.createElement('div');
//   private shadow: ShadowRoot;
//   private interacting = false;
//   private resizeObserver: ResizeObserver;

//   constructor() {
//     super();
//     this.mouseMoveHandler = this.mouseMoveHandler.bind(this);
//     this.mouseUpHandler = this.mouseUpHandler.bind(this);
//     this.initUserInteractionEvents = this.initUserInteractionEvents.bind(this);
//     this.shadow = this.attachShadow({ mode: 'open' });
//     this.preparePointer();
//     this.watchSize();
//     this.initPointerWatch();
//   }

//   private preparePointer(): void {
//     this.pointer.className = 'aria-selector-pointer';
//     this.pointer.style.backgroundColor = 'blue'
//     this.pointer.style.position = 'absolute';
//     this.shadow.append(this.pointer);

//   }

//   connectedCallback() {
//     console.log('passed');
//   }

//   disconnectedCallback() {
//     this.resizeObserver.disconnect();
//   }

//   attributeChangedCallback(attrName: string, oldValue: string, newValue: string): void {
//     switch (attrName) {
//       case 'ps': return this.setPointerSize(newValue);
//     }
//   }

//   private setPointerSize(value: string): void {
//     const size = parseInt(this.getAttribute('ps') || '');
//     this.size = Number.isFinite(size) ? size : this.size;
//     this.pointer.style.height = this.pointer.style.width = `${this.size}px`;
//   }

//   private watchSize(): void {
//     this.resizeObserver = new ResizeObserver(entries => {
//     });
//     this.resizeObserver.observe(this);
//   }

//   private initPointerWatch() {
//     this.pointer.addEventListener('mousedown', this.initUserInteractionEvents);
//     this.pointer.addEventListener('touchstart', this.initUserInteractionEvents);
//   }

//   private initUserInteractionEvents() {
//     if (!this.interacting) {
//       document.addEventListener('mousemove', this.mouseMoveHandler);
//       document.addEventListener('mouseup', this.mouseUpHandler);
//       document.addEventListener('touchmove', this.mouseMoveHandler);
//       document.addEventListener('touchend', this.mouseUpHandler);
//     }
//     this.interacting = true;
//   }

//   private mouseMoveHandler(event: MouseEvent | TouchEvent) {
//     // console.log(Array.prototype.map.call(event.changedTouches, e => e.identifier).join(', '))
//     const pageData = (event as TouchEvent).changedTouches ? (event as TouchEvent).changedTouches[0] : event as MouseEvent;
//     const wrapperRect = this.getBoundingClientRect();
//     const newX = this.getPosition(pageData.clientX, wrapperRect.left, wrapperRect.right);
//     const newY = this.getPosition(pageData.clientY, wrapperRect.top, wrapperRect.bottom);
//     this.pointer.style.left = `${newX}px`;
//     this.pointer.style.top = `${newY}px`;
//     this.trigger(newX, newY);
//   }

//   private getPosition(cur: number, min: number, max: number): number {
//     if (cur < min) {
//       return 0;
//     }
//     if (cur > max) {
//       return max - min;
//     }
//     return cur - min;
//   }

//   private mouseUpHandler() {
//     document.removeEventListener('mousemove', this.mouseMoveHandler);
//     document.removeEventListener('mouseup', this.mouseUpHandler);
//     document.removeEventListener('touchmove', this.mouseMoveHandler);
//     document.removeEventListener('touchend', this.mouseUpHandler);
//     this.interacting = false;
//   }

//   private trigger(x: number, y: number) {
//     this.dispatchEvent(new AriaSelectionEvent(x, y))
//   }
// }

// customElements.define('aria-selector', AriaSelector);
