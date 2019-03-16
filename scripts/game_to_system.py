import sys
import os


class GMReader:
	def __init__(self, input_file):
		self._file_handler = open(input_file, "r")
	
	def read(self):
		identifiers = []
		priorities = []
		players = []
		successors = []
		nicknames = []
		line_counter = 0   #used to skip header line ('parity n')
		for line in self._file_handler.readlines():
			if line_counter >= 1:
				columns = line.split(" ")

				identifiers.append(columns[0])
				priorities.append(columns[1])
				players.append(columns[2])

				current_successors = columns[3].split(",")
				successors.append(current_successors)

				nicknames.append(columns[4])

			line_counter += 1
		return [identifiers, priorities, players, successors, nicknames]


class BooleanSystemGeneratorWriter:
	def __init__(self, winner, gamereader, output_file): #winner = 0 | 1 (i.e. one of the players)
		self._winner = winner
		self._gamereader = gamereader
		self._file_handler = open(output_file, "w")

	def system_generator(self):
		[identifiers, priorities, players, successors, nicknames] = self._gamereader.read()
		#map identifiers to priorities
		ids_priors = {}    #{identifier1:priority1, identifier2:priority2}
		#for i in range(len(identifiers)):
		#	ids_priors[identifiers[i]] = priorities[i]

		identifiers_new_identifiers = {}
		for i in range(len(identifiers)):
			identifiers_new_identifiers[identifiers[i]] = i

		system = []
		for var_id in range(len(priorities)):
			equation_lhs = "x" + str(var_id)
			if int(priorities[var_id]) % 2 == 0:    #even priority
				equation_lhs = equation_lhs + " =max "
			elif int(priorities[var_id]) % 2 != 0:
				equation_lhs = equation_lhs + " =min "  #odd priority

			op = ""
			if players[var_id] == self._winner:
				op = " or "
			else:
				op = " and "

			equation_rhs = ""
			succ_counter = 0
			for succ in successors[var_id]:
				#priority = ids_priors[succ]    #priority of current successor
				if succ_counter == 0:   #
					equation_rhs = "x" + str(identifiers_new_identifiers[succ])
				elif succ_counter > 0:
					equation_rhs = equation_rhs + op + "x" + str(identifiers_new_identifiers[succ])

				succ_counter += 1
			system.append(equation_lhs + equation_rhs)

		return system

	def system_writer(self):
		system = self.system_generator()
		for equation in system:
			self._file_handler.write(equation + "\n")

def main():
	if (len(sys.argv) < 3):
		print "usage: python game_to_system.py [parity_game_file] [output_file] [winner]"
		sys.exit()
	gmr = GMReader(sys.argv[1])
	bsgenwr = BooleanSystemGeneratorWriter(sys.argv[3], gmr, sys.argv[2])
	bsgenwr.system_writer()

if __name__ == "__main__":
	main()
