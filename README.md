-To πρόγραμμα περιλαμβάνει 5 αρχεία. 
1) Το αρχείο client.c που ουσιαστικά είναι η διεργασία του πελάτη
που πληκτρολογεί τον αριθμό γραμμής που θέλει να του επιστρέψει ο 
server από ένα αρχείο που κρατάει.
2) Το αρχείο dispatcher.c που είναι η διεργασία ενδιάμεσου μεταξύ 
client και server, η οποία λαμβάνει τον αριθμό γραμμής που πληκτρολόγησε 
ο client μέσω της shared memory μεταξύ client - dispatcher. Στέλνει
στον server, μέσω 2ης shared memory μεταξύ dispatcher - server τώρα
τον αριθμό γραμμής που δέχτηκε από τον client και περιμένει απάντηση
(το περιεχόμενο της γραμμής) από τον server ώστε να το διαβιβάσει
στην shared memory εαξύ client - dispatcher.
3) Το αρχείο server.c που λαμβάνει τον αριθμό γραμμής (που ζητάει ο 
client) από την shared memory μεταξύ dispatcher - server και επιστρέφει
την γραμμή από το αρχείο στον dispatcher (shared memory) εφόσον υπάρχει.
4) Το αρχείο client-server.h που περιέχει τις βιβλιοθήκες που χρειάζονται
τα 3 προηγύμενα αρχεία, αλλά και τα structs που θα μπουν στις 2
διαμοιραζόμενες μνήμες των client - dispatcher και dispatcher - server.
Επίσης περιέχει μια συνάρτηση που δέχεται όρισμα ένα μήνυμα σε περίπτωση
σφάλματος στο πρόγραμμα.
5) Το αρχείο lines.txt για να ελέγξουμε το πρόγραμμα ότι μας επιστρέφει 
τις γραμμές που ζητάει ο client.


Τεκμηρίωση αρχείων:
1) client.c : Απλώς ανοίγει και προσαρτά στην μνήμη 
του την shared memory του dispatcher.
2) dispatcher.c: Δημιουργεί και προσαρτά τις 2 shared memory's.
Ελέγχει αν ο server τρέχει. Αν όχι, τρέχει τον server με exec όπου στα ορίσματα της
exec περνάει το όνομα της δεύτερης shared memory (dispatcher - server) και το όνομα 
του αρχείου που θα ελέγξει ο server τις γραμμές του.
3) server.c : Φτιάχνει το αρχείο server_on.txt για να δείξει στον dispatcher ότι είναι ενεργός.
Ανοίγει και προσαρτά την shared memory που έφτιαξε ο dispatcher.
Επιστρέφει την γραμμή που ήθελε ο client εφόσον υπάρχει.


Για μεταγλώττιση των αρχείων:
make server
make client
make dispatcher

Για να τρέξει το πρόγραμμα¨
make run-dispatcher ARGS="file_name"

Για να τρέξουν οι clients:
make run-client ARGS="line_number"
