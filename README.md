# PMModelChecker
__PMModelChecker__ is a model checking tool for solving fixpoint equational systems by means of Progress Measures.

_It is known that a number of verification and analysis techniques can be expressed 
in terms of (systems of) fixpoint equations over suitably defined lattices
of information. This is the case for many data flow analyses in settings such
as static analysis of programs, behavioural equivalences in reactive systems and
verification techniques for temporal logics (especially for the **μ-calculus**). Some
recent researches showed that a game-theoretic characterisation of the solution
of equational systems can be devised. Such characterisation is given in terms
of a parity game (a two-player, zero-sum game), which is referred to as **fixpoint
game**. Relying on this theoretical basis, this work aims at developing a generic
game-theoretic algorithm for finding the solution of a given system of equations.
We think that such algorithm can be profitably used for all the aforementioned
verification and model checking tasks (because, as we will see, the winner of the
fixpoint game determines whether a logical formula is satisfied by a given model).
The notion of **Progress Measure**, a tool first devised by Jurdziński for solving parity
games efficiently, will be paramount in establishing which player has a winning
strategy in the play.<br/> 
The main contribution of this work is a generic algorithm and a corresponding
tool that, relying on a description of a set of basic operators over some finite lattice,
is automatically instantied to a solver for systems of fixpoint equations over this
lattice._


# References
Paolo Baldan, Barbara König, Christina Mika-Michalski, and Tommaso Padoan. 2018.<br/> 
_Fixpoint Games on Continuous Lattices._<br/>
https://arxiv.org/abs/1810.11404
