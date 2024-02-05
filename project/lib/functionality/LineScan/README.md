# lib

This library contains all the computer vision functionality for data preprocessing on the embedded device as well as data compression.

The structure is modular and incorporates loose coupling.

The image reader is used for simulation and testing purposes and is located in

```
imageReader/
```

A camera can either be equipped with lineScan or snapshot sensors, and the CV and compression processing differs

The computer vision functionality is implemented in
 
```
lineScan/CVfunctions/
```

and

```
snapshot/CVfunctions/
```

These functions are built such that they can be directly implemented onto the embedded device. In order to test them, we have created

```
lineScan/simulator/
```

and

```
snapshot/simulator/
```

, which utilizes the imageReader and simulates the datastream of the embedded device.

Lastly, we also have incoorporated a compression technique, Quite-Okay Image Format, for both the linescan and snapshot datastreams.