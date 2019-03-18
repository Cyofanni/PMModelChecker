#ifndef MU_CALCULUS_MOVE_GENERATOR_H
#define MU_CALCULUS_MOVE_GENERATOR_H

/*generates basic moves (as strings) for 'box' and 'diamond' operators:
 * accepts an lts.
 */
#include "aldebaran_parser.h"
#include <sstream>

using namespace std;

class MuCalculusMoveGenerator
{
private:
	const char *lts_file;
	int basis_size;

public:
	MuCalculusMoveGenerator(const char *lts_file, int basis_size);

	//generate and return moves as strings, for each basis element
	//if isbox == true => generate moves for 'box'
	vector<string>
	generate_box_diamond_move(bool isbox);
};

#endif
