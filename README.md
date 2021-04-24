# Portfolio

### Gee-nomics (C++)
- Built a class that implemented a Trie-based multi-map
- Worked with files that contained ~3 million bases (e.g. Ferroglobus_placidus's genome), storing the information in tries
- Searched the tries and found exact matches and SNiPs (single nucleotide polymorphisms, i.e. an exact match excluding one base) of short DNA sequences

### MNIST (Pyhton)
- Played around with different machine learning models for the MNIST database
- Used the examples from "Deep Learning with Python" and experimented with changing the number of layers and parameters

### Sokoban Solver (LISP)
- function goal-test checks if the end condition of the search problem has been met, i.e. all the boxes and the keeper are on goal positions
- function next-states takes a state and generates all the possible states that could be reached in one move
- function heuristic305110110 is an admissible heuristic that adds a high cost for moves that would make the game unsolvable
- generated some test cases according to the following mapping: blank=0, wall=1, box=2, keeper=3, goal=4, box+goal=5, keeper+goal=6


### Zombie Dash (C++)
- Built the backend of a game called Zombie Dash by writing classes using polymorphism and managing the interaction between objects
- One example is determining object overlap vs object intersect. The first uses Euclidean distance between the center of two objects and the second uses a bounding box surrounding the objects
