/*
Situation
	Server:
		creates object A
		creates object B
		object A has B* property to B

	client:
		spawns object A
			reads B* property and try to map it but it is not known yet
		spawns object B
			now B* property value of A is known




Situation:
	Server:
		creates object A
		A has B* property which is null 
		create Object X
		a while later A changes B* property to Object X
	
	client:
		client must have object X before replication of B* so server must not send replication of A before X Arrive.



Situation:
	Server:
		Creates Object A
		Creates Object B
		A has B* property To B
		destroys B

	Client:
		A cant destroy B because its not created yet
		Creates A
		Creates B

	Client:

*/


