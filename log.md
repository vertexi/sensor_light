# smart lighting control

- [ ] sensor amp to high or low signal
- [ ] enable interruption to catch the sensor signal jump
  - [ ] when current state is low, when pin got a high signal. interrupt and transmit, vice versa.
  - [ ] when got a high signal set a 10 mins waiting.
- [ ] do a multi receivers multicast. (may be change channel is a way to do thi.)
- [ ] try different frequency and rate, to find a appropriate combination that makes rssi more sensitive to transmitter distance. and do a triangle distance measure algorithm.
