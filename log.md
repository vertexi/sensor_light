# smart lighting control

- [ ] sensor amp to high or low signal
- [ ] enable interruption to catch the sensor signal jump
  - [ ] when current state is low, when pin got a high signal. interrupt and transmit, vice versa.
  - [ ] when got a high signal set a 10 mins waiting.
- [ ] do a multi receivers multicast. (may be change channel is a way to do thi.)
- [ ] try different frequency and rate, to find a appropriate combination that makes rssi more sensitive to transmitter distance. and do a triangle distance measure algorithm.
  > this is another way to do the positioning, so-called multilatering, use the transsimsion, maybe more accurate but need a external time synchronize.

- [ ] Do consistence test with 3k3 ohm.
  > ![consistency_test](./Media/consistency_test.png)

  # 3-10 update
  - [] change the target `0x01` `pos_1` `0x2` `pos_2` `0x3`
  - [] test the RSSI position sensetivity.
