# Light-Recursive-SBDS
Light Recursive SBDS

This is the implementation of the method LReSBDS proposed in the paper "Jimmy Lee and Zichen Zhu.  An Increasing-Nogoods Global Constraint for Symmetry Breaking During Search, Proceedings of the 20th International Conference on Principles and Practice of Constraint Programming (CP 2014), pages 465-480, Lyon, France, September, 2014"

Please use Gecode Solver 4.2.0 to run these files.

Put LReSBDS folder into gecode folder and queens_lresbds.cpp file into example folder.

To run the N-Queens problem (e.g. N=10) using one of the symmetry breaking methods SBDS/ParSBDS/LReSBDS/LDSB:

./queens_resbds -search sbds 10

./queens_resbds -search parsbds 10

./queens_resbds -search lresbds 10

./queens_resbds -search ldsb 10
