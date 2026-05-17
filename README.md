The classic synchronization challenge involving a shared data accessed by multiple processes. 

The synchronization must implement Writer
Preference. This means that once a writer becomes ready to write, no new readers are allowed to
start reading. The goal is to prevent writers from being delayed indefinitely by a continuous stream of
readers.
