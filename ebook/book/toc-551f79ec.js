// Populate the sidebar
//
// This is a script, and not included directly in the page, to control the total size of the book.
// The TOC contains an entry for each page, so if each page includes a copy of the TOC,
// the total size of the page becomes O(n**2).
class MDBookSidebarScrollbox extends HTMLElement {
    constructor() {
        super();
    }
    connectedCallback() {
        this.innerHTML = '<ol class="chapter"><li class="chapter-item expanded "><span class="chapter-link-wrapper"><a href="introduction.html">Introduction</a></span></li><li class="chapter-item expanded "><span class="chapter-link-wrapper"><a href="reading-order.html">Reading order</a></span></li><li class="chapter-item expanded "><li class="part-title">Device labs</li></li><li class="chapter-item expanded "><span class="chapter-link-wrapper"><a href="tracks/device.html"><strong aria-hidden="true">1.</strong> Device labs</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex00-hello-device/index.html"><strong aria-hidden="true">1.1.</strong> ex00-hello-device</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex00-hello-device/step-01-lcd-hello.html"><strong aria-hidden="true">1.1.1.</strong> step-01-lcd-hello</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex00-hello-device/step-02-sound-pad.html"><strong aria-hidden="true">1.1.2.</strong> step-02-sound-pad</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex00-hello-device/step-03-two-tasks.html"><strong aria-hidden="true">1.1.3.</strong> step-03-two-tasks</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex00-hello-device/step-04-imu-status.html"><strong aria-hidden="true">1.1.4.</strong> step-04-imu-status</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex00-hello-device/step-05-boot-leds.html"><strong aria-hidden="true">1.1.5.</strong> step-05-boot-leds</a></span></li></ol><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex01-spirit-level/index.html"><strong aria-hidden="true">1.2.</strong> ex01-spirit-level</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex01-spirit-level/step-01-draw-tube.html"><strong aria-hidden="true">1.2.1.</strong> step-01-draw-tube</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex01-spirit-level/step-02-imu-bubble.html"><strong aria-hidden="true">1.2.2.</strong> step-02-imu-bubble</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex01-spirit-level/step-03-busy-loop.html"><strong aria-hidden="true">1.2.3.</strong> step-03-busy-loop</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex01-spirit-level/step-04-split-queue.html"><strong aria-hidden="true">1.2.4.</strong> step-04-split-queue</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex01-spirit-level/step-05-level-cue.html"><strong aria-hidden="true">1.2.5.</strong> step-05-level-cue</a></span></li></ol><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex02-motion-alarm/index.html"><strong aria-hidden="true">1.3.</strong> ex02-motion-alarm</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex02-motion-alarm/step-01-armed-face.html"><strong aria-hidden="true">1.3.1.</strong> step-01-armed-face</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex02-motion-alarm/step-02-shake-detect.html"><strong aria-hidden="true">1.3.2.</strong> step-02-shake-detect</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex02-motion-alarm/step-03-busy-alarm.html"><strong aria-hidden="true">1.3.3.</strong> step-03-busy-alarm</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex02-motion-alarm/step-04-queue-audio.html"><strong aria-hidden="true">1.3.4.</strong> step-04-queue-audio</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex02-motion-alarm/step-05-arm-disarm.html"><strong aria-hidden="true">1.3.5.</strong> step-05-arm-disarm</a></span></li></ol><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex03-reaction-game/index.html"><strong aria-hidden="true">1.4.</strong> ex03-reaction-game</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex03-reaction-game/step-01-game-face.html"><strong aria-hidden="true">1.4.1.</strong> step-01-game-face</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex03-reaction-game/step-02-go-and-tap.html"><strong aria-hidden="true">1.4.2.</strong> step-02-go-and-tap</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex03-reaction-game/step-03-busy-wait.html"><strong aria-hidden="true">1.4.3.</strong> step-03-busy-wait</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex03-reaction-game/step-04-timer-go.html"><strong aria-hidden="true">1.4.4.</strong> step-04-timer-go</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex03-reaction-game/step-05-score-audio.html"><strong aria-hidden="true">1.4.5.</strong> step-05-score-audio</a></span></li></ol><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex04-metronome/index.html"><strong aria-hidden="true">1.5.</strong> ex04-metronome</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex04-metronome/step-01-bpm-face.html"><strong aria-hidden="true">1.5.1.</strong> step-01-bpm-face</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex04-metronome/step-02-ui-click.html"><strong aria-hidden="true">1.5.2.</strong> step-02-ui-click</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex04-metronome/step-03-feel-jitter.html"><strong aria-hidden="true">1.5.3.</strong> step-03-feel-jitter</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex04-metronome/step-04-tick-task.html"><strong aria-hidden="true">1.5.4.</strong> step-04-tick-task</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex04-metronome/step-05-bpm-control.html"><strong aria-hidden="true">1.5.5.</strong> step-05-bpm-control</a></span></li></ol><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex05-vibration-scope/index.html"><strong aria-hidden="true">1.6.</strong> ex05-vibration-scope</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex05-vibration-scope/step-01-scope-face.html"><strong aria-hidden="true">1.6.1.</strong> step-01-scope-face</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex05-vibration-scope/step-02-imu-scope.html"><strong aria-hidden="true">1.6.2.</strong> step-02-imu-scope</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex05-vibration-scope/step-03-ui-100hz.html"><strong aria-hidden="true">1.6.3.</strong> step-03-ui-100hz</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex05-vibration-scope/step-04-split-queue.html"><strong aria-hidden="true">1.6.4.</strong> step-04-split-queue</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex05-vibration-scope/step-05-drop-stats.html"><strong aria-hidden="true">1.6.5.</strong> step-05-drop-stats</a></span></li></ol><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex06-task-monitor/index.html"><strong aria-hidden="true">1.7.</strong> ex06-task-monitor</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex06-task-monitor/step-01-name-face.html"><strong aria-hidden="true">1.7.1.</strong> step-01-name-face</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex06-task-monitor/step-02-live-names.html"><strong aria-hidden="true">1.7.2.</strong> step-02-live-names</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex06-task-monitor/step-03-state-prio.html"><strong aria-hidden="true">1.7.3.</strong> step-03-state-prio</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex06-task-monitor/step-04-stack-hwm.html"><strong aria-hidden="true">1.7.4.</strong> step-04-stack-hwm</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/device/ex06-task-monitor/step-05-queue-depth.html"><strong aria-hidden="true">1.7.5.</strong> step-05-queue-depth</a></span></li></ol></li></ol><li class="chapter-item expanded "><li class="part-title">Special labs</li></li><li class="chapter-item expanded "><span class="chapter-link-wrapper"><a href="tracks/special.html"><strong aria-hidden="true">2.</strong> Special labs</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex01-mutex-beep/index.html"><strong aria-hidden="true">2.1.</strong> ex01-mutex-beep</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex01-mutex-beep/step-01-two-voices.html"><strong aria-hidden="true">2.1.1.</strong> step-01-two-voices</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex01-mutex-beep/step-02-overlap-crash.html"><strong aria-hidden="true">2.1.2.</strong> step-02-overlap-crash</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex01-mutex-beep/step-03-mutex-guard.html"><strong aria-hidden="true">2.1.3.</strong> step-03-mutex-guard</a></span></li></ol><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex02-boot-isr/index.html"><strong aria-hidden="true">2.2.</strong> ex02-boot-isr</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex02-boot-isr/step-01-poll-boot.html"><strong aria-hidden="true">2.2.1.</strong> step-01-poll-boot</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex02-boot-isr/step-02-isr-busy.html"><strong aria-hidden="true">2.2.2.</strong> step-02-isr-busy</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex02-boot-isr/step-03-isr-notify.html"><strong aria-hidden="true">2.2.3.</strong> step-03-isr-notify</a></span></li></ol><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex03-priority-starve/index.html"><strong aria-hidden="true">2.3.</strong> ex03-priority-starve</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex03-priority-starve/step-01-ui-worker.html"><strong aria-hidden="true">2.3.1.</strong> step-01-ui-worker</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex03-priority-starve/step-02-high-prio-hog.html"><strong aria-hidden="true">2.3.2.</strong> step-02-high-prio-hog</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex03-priority-starve/step-03-yield-fix.html"><strong aria-hidden="true">2.3.3.</strong> step-03-yield-fix</a></span></li></ol><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex04-counting-sem/index.html"><strong aria-hidden="true">2.4.</strong> ex04-counting-sem</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex04-counting-sem/step-01-three-slots.html"><strong aria-hidden="true">2.4.1.</strong> step-01-three-slots</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex04-counting-sem/step-02-oversubscribe.html"><strong aria-hidden="true">2.4.2.</strong> step-02-oversubscribe</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex04-counting-sem/step-03-sem-pool.html"><strong aria-hidden="true">2.4.3.</strong> step-03-sem-pool</a></span></li></ol><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex05-event-group/index.html"><strong aria-hidden="true">2.5.</strong> ex05-event-group</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex05-event-group/step-01-checklist.html"><strong aria-hidden="true">2.5.1.</strong> step-01-checklist</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex05-event-group/step-02-one-bit-arm.html"><strong aria-hidden="true">2.5.2.</strong> step-02-one-bit-arm</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex05-event-group/step-03-wait-bits.html"><strong aria-hidden="true">2.5.3.</strong> step-03-wait-bits</a></span></li></ol><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex06-reload-timer/index.html"><strong aria-hidden="true">2.6.</strong> ex06-reload-timer</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex06-reload-timer/step-01-manual-pulse.html"><strong aria-hidden="true">2.6.1.</strong> step-01-manual-pulse</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex06-reload-timer/step-02-delay-beat.html"><strong aria-hidden="true">2.6.2.</strong> step-02-delay-beat</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/special/ex06-reload-timer/step-03-auto-reload.html"><strong aria-hidden="true">2.6.3.</strong> step-03-auto-reload</a></span></li></ol></li></ol><li class="chapter-item expanded "><li class="part-title">RTOS labs</li></li><li class="chapter-item expanded "><span class="chapter-link-wrapper"><a href="tracks/rtos.html"><strong aria-hidden="true">3.</strong> RTOS labs</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex00-task-creation/index.html"><strong aria-hidden="true">3.1.</strong> ex00-task-creation</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex01-task-affinity/index.html"><strong aria-hidden="true">3.2.</strong> ex01-task-affinity</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex02-os-tick/index.html"><strong aria-hidden="true">3.3.</strong> ex02-os-tick</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex03-imu-pipeline/index.html"><strong aria-hidden="true">3.4.</strong> ex03-imu-pipeline</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex04-task-priority/index.html"><strong aria-hidden="true">3.5.</strong> ex04-task-priority</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex05-task-suspend/index.html"><strong aria-hidden="true">3.6.</strong> ex05-task-suspend</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex06-mutex-canvas/index.html"><strong aria-hidden="true">3.7.</strong> ex06-mutex-canvas</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex07-counting-sem/index.html"><strong aria-hidden="true">3.8.</strong> ex07-counting-sem</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex08-event-group/index.html"><strong aria-hidden="true">3.9.</strong> ex08-event-group</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex09-reload-timer/index.html"><strong aria-hidden="true">3.10.</strong> ex09-reload-timer</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex10-boot-isr/index.html"><strong aria-hidden="true">3.11.</strong> ex10-boot-isr</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex11-imu-racer/index.html"><strong aria-hidden="true">3.12.</strong> ex11-imu-racer</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex12-brick-break/index.html"><strong aria-hidden="true">3.13.</strong> ex12-brick-break</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex13-space-shoot/index.html"><strong aria-hidden="true">3.14.</strong> ex13-space-shoot</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex14-asteroids/index.html"><strong aria-hidden="true">3.15.</strong> ex14-asteroids</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/rtos/ex15-frogger/index.html"><strong aria-hidden="true">3.16.</strong> ex15-frogger</a></span></li></ol><li class="chapter-item expanded "><li class="part-title">Demo labs</li></li><li class="chapter-item expanded "><span class="chapter-link-wrapper"><a href="tracks/demo.html"><strong aria-hidden="true">4.</strong> Demo labs</a><a class="chapter-fold-toggle"><div>❱</div></a></span><ol class="section"><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex01-label/index.html"><strong aria-hidden="true">4.1.</strong> ex01-label</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex02-led/index.html"><strong aria-hidden="true">4.2.</strong> ex02-led</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex03-term/index.html"><strong aria-hidden="true">4.3.</strong> ex03-term</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex04-switch/index.html"><strong aria-hidden="true">4.4.</strong> ex04-switch</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex05-button/index.html"><strong aria-hidden="true">4.5.</strong> ex05-button</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex06-slider/index.html"><strong aria-hidden="true">4.6.</strong> ex06-slider</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex07-checkbox/index.html"><strong aria-hidden="true">4.7.</strong> ex07-checkbox</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex08-radio/index.html"><strong aria-hidden="true">4.8.</strong> ex08-radio</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex09-form/index.html"><strong aria-hidden="true">4.9.</strong> ex09-form</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex10-log/index.html"><strong aria-hidden="true">4.10.</strong> ex10-log</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex11-gauge/index.html"><strong aria-hidden="true">4.11.</strong> ex11-gauge</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex12-param/index.html"><strong aria-hidden="true">4.12.</strong> ex12-param</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex13-topics-ui/index.html"><strong aria-hidden="true">4.13.</strong> ex13-topics-ui</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex14-progress/index.html"><strong aria-hidden="true">4.14.</strong> ex14-progress</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex15-chart/index.html"><strong aria-hidden="true">4.15.</strong> ex15-chart</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex16-imu-viz/index.html"><strong aria-hidden="true">4.16.</strong> ex16-imu-viz</a></span></li><li class="chapter-item "><span class="chapter-link-wrapper"><a href="generated/demo/ex17-mic-viz/index.html"><strong aria-hidden="true">4.17.</strong> ex17-mic-viz</a></span></li></ol></li></ol>';
        // Set the current, active page, and reveal it if it's hidden
        let current_page = document.location.href.toString().split('#')[0].split('?')[0];
        if (current_page.endsWith('/')) {
            current_page += 'index.html';
        }
        const links = Array.prototype.slice.call(this.querySelectorAll('a'));
        const l = links.length;
        for (let i = 0; i < l; ++i) {
            const link = links[i];
            const href = link.getAttribute('href');
            if (href && !href.startsWith('#') && !/^(?:[a-z+]+:)?\/\//.test(href)) {
                link.href = path_to_root + href;
            }
            // The 'index' page is supposed to alias the first chapter in the book.
            // Check both with and without the '.html' suffix to be robust against pretty URLs
            if (link.href.replace(/\.html$/, '') === current_page.replace(/\.html$/, '')
                || i === 0
                && path_to_root === ''
                && current_page.endsWith('/index.html')) {
                link.classList.add('active');
                let parent = link.parentElement;
                while (parent) {
                    if (parent.tagName === 'LI' && parent.classList.contains('chapter-item')) {
                        parent.classList.add('expanded');
                    }
                    parent = parent.parentElement;
                }
            }
        }
        // Track and set sidebar scroll position
        this.addEventListener('click', e => {
            if (e.target.tagName === 'A') {
                const clientRect = e.target.getBoundingClientRect();
                const sidebarRect = this.getBoundingClientRect();
                sessionStorage.setItem('sidebar-scroll-offset', clientRect.top - sidebarRect.top);
            }
        }, { passive: true });
        const sidebarScrollOffset = sessionStorage.getItem('sidebar-scroll-offset');
        sessionStorage.removeItem('sidebar-scroll-offset');
        if (sidebarScrollOffset !== null) {
            // preserve sidebar scroll position when navigating via links within sidebar
            const activeSection = this.querySelector('.active');
            if (activeSection) {
                const clientRect = activeSection.getBoundingClientRect();
                const sidebarRect = this.getBoundingClientRect();
                const currentOffset = clientRect.top - sidebarRect.top;
                this.scrollTop += currentOffset - parseFloat(sidebarScrollOffset);
            }
        } else {
            // scroll sidebar to current active section when navigating via
            // 'next/previous chapter' buttons
            const activeSection = document.querySelector('#mdbook-sidebar .active');
            if (activeSection) {
                activeSection.scrollIntoView({ block: 'center' });
            }
        }
        // Toggle buttons
        const sidebarAnchorToggles = document.querySelectorAll('.chapter-fold-toggle');
        function toggleSection(ev) {
            ev.currentTarget.parentElement.parentElement.classList.toggle('expanded');
        }
        Array.from(sidebarAnchorToggles).forEach(el => {
            el.addEventListener('click', toggleSection);
        });
    }
}
window.customElements.define('mdbook-sidebar-scrollbox', MDBookSidebarScrollbox);


// ---------------------------------------------------------------------------
// Support for dynamically adding headers to the sidebar.

(function() {
    // This is used to detect which direction the page has scrolled since the
    // last scroll event.
    let lastKnownScrollPosition = 0;
    // This is the threshold in px from the top of the screen where it will
    // consider a header the "current" header when scrolling down.
    const defaultDownThreshold = 150;
    // Same as defaultDownThreshold, except when scrolling up.
    const defaultUpThreshold = 300;
    // The threshold is a virtual horizontal line on the screen where it
    // considers the "current" header to be above the line. The threshold is
    // modified dynamically to handle headers that are near the bottom of the
    // screen, and to slightly offset the behavior when scrolling up vs down.
    let threshold = defaultDownThreshold;
    // This is used to disable updates while scrolling. This is needed when
    // clicking the header in the sidebar, which triggers a scroll event. It
    // is somewhat finicky to detect when the scroll has finished, so this
    // uses a relatively dumb system of disabling scroll updates for a short
    // time after the click.
    let disableScroll = false;
    // Array of header elements on the page.
    let headers;
    // Array of li elements that are initially collapsed headers in the sidebar.
    // I'm not sure why eslint seems to have a false positive here.
    // eslint-disable-next-line prefer-const
    let headerToggles = [];
    // This is a debugging tool for the threshold which you can enable in the console.
    let thresholdDebug = false;

    // Updates the threshold based on the scroll position.
    function updateThreshold() {
        const scrollTop = window.pageYOffset || document.documentElement.scrollTop;
        const windowHeight = window.innerHeight;
        const documentHeight = document.documentElement.scrollHeight;

        // The number of pixels below the viewport, at most documentHeight.
        // This is used to push the threshold down to the bottom of the page
        // as the user scrolls towards the bottom.
        const pixelsBelow = Math.max(0, documentHeight - (scrollTop + windowHeight));
        // The number of pixels above the viewport, at least defaultDownThreshold.
        // Similar to pixelsBelow, this is used to push the threshold back towards
        // the top when reaching the top of the page.
        const pixelsAbove = Math.max(0, defaultDownThreshold - scrollTop);
        // How much the threshold should be offset once it gets close to the
        // bottom of the page.
        const bottomAdd = Math.max(0, windowHeight - pixelsBelow - defaultDownThreshold);
        let adjustedBottomAdd = bottomAdd;

        // Adjusts bottomAdd for a small document. The calculation above
        // assumes the document is at least twice the windowheight in size. If
        // it is less than that, then bottomAdd needs to be shrunk
        // proportional to the difference in size.
        if (documentHeight < windowHeight * 2) {
            const maxPixelsBelow = documentHeight - windowHeight;
            const t = 1 - pixelsBelow / Math.max(1, maxPixelsBelow);
            const clamp = Math.max(0, Math.min(1, t));
            adjustedBottomAdd *= clamp;
        }

        let scrollingDown = true;
        if (scrollTop < lastKnownScrollPosition) {
            scrollingDown = false;
        }

        if (scrollingDown) {
            // When scrolling down, move the threshold up towards the default
            // downwards threshold position. If near the bottom of the page,
            // adjustedBottomAdd will offset the threshold towards the bottom
            // of the page.
            const amountScrolledDown = scrollTop - lastKnownScrollPosition;
            const adjustedDefault = defaultDownThreshold + adjustedBottomAdd;
            threshold = Math.max(adjustedDefault, threshold - amountScrolledDown);
        } else {
            // When scrolling up, move the threshold down towards the default
            // upwards threshold position. If near the bottom of the page,
            // quickly transition the threshold back up where it normally
            // belongs.
            const amountScrolledUp = lastKnownScrollPosition - scrollTop;
            const adjustedDefault = defaultUpThreshold - pixelsAbove
                + Math.max(0, adjustedBottomAdd - defaultDownThreshold);
            threshold = Math.min(adjustedDefault, threshold + amountScrolledUp);
        }

        if (documentHeight <= windowHeight) {
            threshold = 0;
        }

        if (thresholdDebug) {
            const id = 'mdbook-threshold-debug-data';
            let data = document.getElementById(id);
            if (data === null) {
                data = document.createElement('div');
                data.id = id;
                data.style.cssText = `
                    position: fixed;
                    top: 50px;
                    right: 10px;
                    background-color: 0xeeeeee;
                    z-index: 9999;
                    pointer-events: none;
                `;
                document.body.appendChild(data);
            }
            data.innerHTML = `
                <table>
                  <tr><td>documentHeight</td><td>${documentHeight.toFixed(1)}</td></tr>
                  <tr><td>windowHeight</td><td>${windowHeight.toFixed(1)}</td></tr>
                  <tr><td>scrollTop</td><td>${scrollTop.toFixed(1)}</td></tr>
                  <tr><td>pixelsAbove</td><td>${pixelsAbove.toFixed(1)}</td></tr>
                  <tr><td>pixelsBelow</td><td>${pixelsBelow.toFixed(1)}</td></tr>
                  <tr><td>bottomAdd</td><td>${bottomAdd.toFixed(1)}</td></tr>
                  <tr><td>adjustedBottomAdd</td><td>${adjustedBottomAdd.toFixed(1)}</td></tr>
                  <tr><td>scrollingDown</td><td>${scrollingDown}</td></tr>
                  <tr><td>threshold</td><td>${threshold.toFixed(1)}</td></tr>
                </table>
            `;
            drawDebugLine();
        }

        lastKnownScrollPosition = scrollTop;
    }

    function drawDebugLine() {
        if (!document.body) {
            return;
        }
        const id = 'mdbook-threshold-debug-line';
        const existingLine = document.getElementById(id);
        if (existingLine) {
            existingLine.remove();
        }
        const line = document.createElement('div');
        line.id = id;
        line.style.cssText = `
            position: fixed;
            top: ${threshold}px;
            left: 0;
            width: 100vw;
            height: 2px;
            background-color: red;
            z-index: 9999;
            pointer-events: none;
        `;
        document.body.appendChild(line);
    }

    function mdbookEnableThresholdDebug() {
        thresholdDebug = true;
        updateThreshold();
        drawDebugLine();
    }

    window.mdbookEnableThresholdDebug = mdbookEnableThresholdDebug;

    // Updates which headers in the sidebar should be expanded. If the current
    // header is inside a collapsed group, then it, and all its parents should
    // be expanded.
    function updateHeaderExpanded(currentA) {
        // Add expanded to all header-item li ancestors.
        let current = currentA.parentElement;
        while (current) {
            if (current.tagName === 'LI' && current.classList.contains('header-item')) {
                current.classList.add('expanded');
            }
            current = current.parentElement;
        }
    }

    // Updates which header is marked as the "current" header in the sidebar.
    // This is done with a virtual Y threshold, where headers at or below
    // that line will be considered the current one.
    function updateCurrentHeader() {
        if (!headers || !headers.length) {
            return;
        }

        // Reset the classes, which will be rebuilt below.
        const els = document.getElementsByClassName('current-header');
        for (const el of els) {
            el.classList.remove('current-header');
        }
        for (const toggle of headerToggles) {
            toggle.classList.remove('expanded');
        }

        // Find the last header that is above the threshold.
        let lastHeader = null;
        for (const header of headers) {
            const rect = header.getBoundingClientRect();
            if (rect.top <= threshold) {
                lastHeader = header;
            } else {
                break;
            }
        }
        if (lastHeader === null) {
            lastHeader = headers[0];
            const rect = lastHeader.getBoundingClientRect();
            const windowHeight = window.innerHeight;
            if (rect.top >= windowHeight) {
                return;
            }
        }

        // Get the anchor in the summary.
        const href = '#' + lastHeader.id;
        const a = [...document.querySelectorAll('.header-in-summary')]
            .find(element => element.getAttribute('href') === href);
        if (!a) {
            return;
        }

        a.classList.add('current-header');

        updateHeaderExpanded(a);
    }

    // Updates which header is "current" based on the threshold line.
    function reloadCurrentHeader() {
        if (disableScroll) {
            return;
        }
        updateThreshold();
        updateCurrentHeader();
    }


    // When clicking on a header in the sidebar, this adjusts the threshold so
    // that it is located next to the header. This is so that header becomes
    // "current".
    function headerThresholdClick(event) {
        // See disableScroll description why this is done.
        disableScroll = true;
        setTimeout(() => {
            disableScroll = false;
        }, 100);
        // requestAnimationFrame is used to delay the update of the "current"
        // header until after the scroll is done, and the header is in the new
        // position.
        requestAnimationFrame(() => {
            requestAnimationFrame(() => {
                // Closest is needed because if it has child elements like <code>.
                const a = event.target.closest('a');
                const href = a.getAttribute('href');
                const targetId = href.substring(1);
                const targetElement = document.getElementById(targetId);
                if (targetElement) {
                    threshold = targetElement.getBoundingClientRect().bottom;
                    updateCurrentHeader();
                }
            });
        });
    }

    // Takes the nodes from the given head and copies them over to the
    // destination, along with some filtering.
    function filterHeader(source, dest) {
        const clone = source.cloneNode(true);
        clone.querySelectorAll('mark').forEach(mark => {
            mark.replaceWith(...mark.childNodes);
        });
        dest.append(...clone.childNodes);
    }

    // Scans page for headers and adds them to the sidebar.
    document.addEventListener('DOMContentLoaded', function() {
        const activeSection = document.querySelector('#mdbook-sidebar .active');
        if (activeSection === null) {
            return;
        }

        const main = document.getElementsByTagName('main')[0];
        headers = Array.from(main.querySelectorAll('h2, h3, h4, h5, h6'))
            .filter(h => h.id !== '' && h.children.length && h.children[0].tagName === 'A');

        if (headers.length === 0) {
            return;
        }

        // Build a tree of headers in the sidebar.

        const stack = [];

        const firstLevel = parseInt(headers[0].tagName.charAt(1));
        for (let i = 1; i < firstLevel; i++) {
            const ol = document.createElement('ol');
            ol.classList.add('section');
            if (stack.length > 0) {
                stack[stack.length - 1].ol.appendChild(ol);
            }
            stack.push({level: i + 1, ol: ol});
        }

        // The level where it will start folding deeply nested headers.
        const foldLevel = 3;

        for (let i = 0; i < headers.length; i++) {
            const header = headers[i];
            const level = parseInt(header.tagName.charAt(1));

            const currentLevel = stack[stack.length - 1].level;
            if (level > currentLevel) {
                // Begin nesting to this level.
                for (let nextLevel = currentLevel + 1; nextLevel <= level; nextLevel++) {
                    const ol = document.createElement('ol');
                    ol.classList.add('section');
                    const last = stack[stack.length - 1];
                    const lastChild = last.ol.lastChild;
                    // Handle the case where jumping more than one nesting
                    // level, which doesn't have a list item to place this new
                    // list inside of.
                    if (lastChild) {
                        lastChild.appendChild(ol);
                    } else {
                        last.ol.appendChild(ol);
                    }
                    stack.push({level: nextLevel, ol: ol});
                }
            } else if (level < currentLevel) {
                while (stack.length > 1 && stack[stack.length - 1].level > level) {
                    stack.pop();
                }
            }

            const li = document.createElement('li');
            li.classList.add('header-item');
            li.classList.add('expanded');
            if (level < foldLevel) {
                li.classList.add('expanded');
            }
            const span = document.createElement('span');
            span.classList.add('chapter-link-wrapper');
            const a = document.createElement('a');
            span.appendChild(a);
            a.href = '#' + header.id;
            a.classList.add('header-in-summary');
            filterHeader(header.children[0], a);
            a.addEventListener('click', headerThresholdClick);
            const nextHeader = headers[i + 1];
            if (nextHeader !== undefined) {
                const nextLevel = parseInt(nextHeader.tagName.charAt(1));
                if (nextLevel > level && level >= foldLevel) {
                    const toggle = document.createElement('a');
                    toggle.classList.add('chapter-fold-toggle');
                    toggle.classList.add('header-toggle');
                    toggle.addEventListener('click', () => {
                        li.classList.toggle('expanded');
                    });
                    const toggleDiv = document.createElement('div');
                    toggleDiv.textContent = '❱';
                    toggle.appendChild(toggleDiv);
                    span.appendChild(toggle);
                    headerToggles.push(li);
                }
            }
            li.appendChild(span);

            const currentParent = stack[stack.length - 1];
            currentParent.ol.appendChild(li);
        }

        const onThisPage = document.createElement('div');
        onThisPage.classList.add('on-this-page');
        onThisPage.append(stack[0].ol);
        const activeItemSpan = activeSection.parentElement;
        activeItemSpan.after(onThisPage);
    });

    document.addEventListener('DOMContentLoaded', reloadCurrentHeader);
    document.addEventListener('scroll', reloadCurrentHeader, { passive: true });
})();

