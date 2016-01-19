# threadLibrary

p1t includes a fully functional thread library, which allows a user to run their code on multiple threads simultaneously. 
It is also equipped with a test suite to ensure that it works as it should.

p1d puts the thread library into practice. deli.cc is a virtual representation of a deli, that uses a thread library to 
create multiple sandwhichMakers at a time, so that they can fulill orders simultaneously. Though it currently uses a built
in thread library, if p1t is imported into deli.cc and "included" at the top of the file, it can run using the thread library
we created.

I worked on this project with Nick Widmaier and Olivia Zhu.
