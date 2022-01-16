# Portfolio

### Gee-nomics (C++)
- Built a class that implemented a Trie-based multi-map
- Worked with files that contained ~3 million bases (e.g. Ferroglobus_placidus's genome), storing the information in tries
- Searched the tries and found exact matches and SNiPs (single nucleotide polymorphisms, i.e. an exact match excluding one base) of short DNA sequences

### Modeling and Predicting Stroke (Python in Jupyter Notebook)
- Visualized and preprocessed the data (imputing, resampling, normalizing, encoding features, and crossing features)
- Trained models (logistic regression, neural network, spuport vector machine, and decision tree) and evaluated accuracy
- Applied techniques such as PCA and bootstrap aggregation

### Sokoban Solver (LISP)
- Checked if the end condition of the search problem had been met, i.e. all the boxes and the keeper are on goal positions (goal-test)
- Took the current state and generated all the possible states that could be reached in one move (next-states)
- Wrote an admissible heuristic that adds a high cost for moves that would make the game unsolvable (heuristic305110110)
- Generated some test cases according to the following mapping: blank=0, wall=1, box=2, keeper=3, goal=4, box+goal=5, keeper+goal=6


### Zombie Dash (C++)
- Built the backend of a game called Zombie Dash by writing classes using polymorphism and managing the interaction between objects
- One example is determining object overlap vs object intersect. The first uses Euclidean distance between the center of two objects and the second uses a bounding box surrounding the objects
