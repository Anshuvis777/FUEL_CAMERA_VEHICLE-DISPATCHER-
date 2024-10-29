<img width="920" alt="image (1)" src="https://github.com/user-attachments/assets/9e6f42a6-606c-4bc7-b2be-9457b9fef17e"># FUEL_CAMERA_VEHICLE-DISPATCHER-
COMPUTER NETWORKS  -  PROJECT USING NETWORK PROGRAMMING 
Fuel Filling Station

The following persons/active working elements are in a fuel filling station scenario.

Fuel dispenser - D-dispenses petrol or diesel or gas (i.e. three services)

Two Billing persons - B1, B2

An Active Surveillance camera - SC

Vehicle/Client/Customer - V

Initially x litres of petrol, y litres of diesel, z litres of gas available at the station. When a vehicle V enters into the station (gets connected) for a particular fuel, then D services

it with that fuel, updates the value of that fuel (i.e. petrol/ diesel/gas) and passes it to one of the Billing persons for payment of bill. There is only one common path between D, B1, B2 for getting a V passed for payment. D sees that Vs are passed to B1 and B2 alternatively for paying the bill. That means V1 will be passed to B1, V2 will be passed to B2, V3 to B1, V4 to B2 and so on. The process B, collects bill amount from V and records that information in a bill book. The entry of bill book may be id of vehicle V and the amount. Sometimes, a B; may forget to record or deceive not to record in the bill book for a payment entry.

The active Surveillance camera - SC observes all the vehicles entering into the station and stores the ids of them. When a total of 10 vehicles got served, it checks/tallies its data with the data in bill book (ie. ids of all Vs ). If the check is not correct, then SC notifies D to stop serving. Otherwise the filling service continues.

Whenever any fuel value reaches zero litres, the dispenser - D informs the same information

(ie. that particular fuel is not available) to all the vehicles served so far by it.

It is clear that D, B1, B2 and SC belong to the same filling station and are unrelated processes,
and vehicle V is entirely from a different location.

<img width="920" alt="image (1)" src="https://github.com/user-attachments/assets/1334e300-ebd6-4095-b145-87ba6d08b124">

