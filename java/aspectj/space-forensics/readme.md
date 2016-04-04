# Introducing HAL

You are being asked to investigate the occurrences that took place on a remote space station in which
both crew members, Dr. David Bowman and Dr. Frank Poole have been found dead. Forensics show
that they were suffocated. You have access to the black box of the station (whose scenario is shown
below on the next page).

Eventually you gain access to the source code of the on-board computer that is composed only of
five Java classes.

As is, HAL’s implementation and your findings (simulation and logs) do not match. You realize that
HAL’s true mission is hidden to crew members. You come to the conclusion that HAL included
aspectual behavior that allowed it to protect itself from a shutdown and take over the station while
killing its crew members. Your assignment is to recreate HAL’s exact behavior as the one illustrated
in the simulation and in the logs. To do that, you need to develop the following three aspects listed
below in alphabetical order:

Authorization This aspect performs two tasks: First, it intercepts requests by crew members to
obtain information on the purpose of the mission. Second, it intercepts requests by crew
members to shut down the on-board computer.

LifeSupport This aspect performs two tasks. First, it extends the definition of class Crew by
introducing an attribute to indicate whether or not an instance is alive or dead (assume that
this is connected to some mechanical device that controls the member’s oxygen supply) together
with functionality to manipulate it. Second, it intercepts all messages sent to the on-board
computer by crew members and performs filtering.

Logger This aspect creates a text file system-logs.txt to keep a log of all messages sent within
the system together with the relative time in milliseconds (modulo 1000). To avoid clutter, the
aspect performs some filtering on which messages are to be logged.

Pay very close attention to the simulation and to the logs with respect to the core functionality of
the application (the source code provided). Note that you may not under any circumstances modify
the core functionality. Further, note that to simulate the fact that a person is dead, not only we
assign an appropriate value to its state, but we ignore any messages that this person may be sending
from the time of being killed onward.

