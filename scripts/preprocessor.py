#accept a pretty-written system and the basis with readable names

import sys
import os
import re

class BasisReader:
	def __init__(self, basis_file):
		#get basis file as input
		self._file_handler = open(basis_file, "r")

	def read(self):
		names_indices = {}
		line_counter = 0
		for line in self._file_handler.readlines():
			line = line.replace(" ", "")
			line = line.replace("\t", "")
			line = line.replace('\n','')
			names_indices[line] = line_counter
			line_counter += 1
		return names_indices    #{"a":0, "b":1 etc...}


class ConvertedSystemWriter:
	def __init__(self, br, system_file, output_file):
		self._basis_reader = br
		self._system_file = open(system_file, "r")
		self._output_file_path = output_file

	def new_system_generator(self):
		syntax_error = -1
		line_counter = 0
		basis_names_indices = self._basis_reader.read()
		new_lines = []
		for line in self._system_file.readlines():
			line = line.strip()
			if len(line) > 0:
				if not (line.startswith('#') or re.match('^x[0-9]+', line) or line.startswith("phi")):
					syntax_error = line_counter
					break

			if line[0:3] == "phi":
                		converted_line = ""
				#handle 'phi' identifier
				first_underscore_position = 3
				second_underscore_position = line.find('_', 4)
				basis_identifier = line[first_underscore_position+1 : second_underscore_position]
				basis_index = basis_names_indices[basis_identifier]

				line = line[0 : first_underscore_position+1] + str(basis_index) + line[second_underscore_position :]

				#end of 'phi' identifier processing
				#now process atoms at the rhs
				find_from = line.find('=')  #start looking for '[' after the '=' sign
				while line.find('[', find_from+1) != -1:
					find_from = line.find('[', find_from+1)
					next_comma_position = line.find(',', find_from)
					basis_identifier = line[find_from+1 : next_comma_position].replace(" ", "").replace("\t", "")
					basis_index = basis_names_indices[basis_identifier]
					next_bracket_position = line.find(']', find_from)
					line = line[0:find_from + 1] + "b_" + str(basis_index) + line[next_comma_position : next_bracket_position].replace(" ", "") \
						+ line[next_bracket_position :]

				new_lines.append(line + '\n')

			else:
				if not (line.startswith('#')):
					new_lines.append(line + '\n')

			line_counter += 1

		return [new_lines, syntax_error]

	def new_system_writer(self):
		[new_lines, syntax_error] = self.new_system_generator()
		if syntax_error == -1:
			output = open(self._output_file_path, "w")
			for line in new_lines:
				output.write(line)

		return syntax_error


def main():
	if (len(sys.argv) < 4):
		print "usage: python " + sys.argv[0] + " [basis_file] [system_file] [output_file]"
		sys.exit()

	if os.path.isfile(sys.argv[1]) == True and os.path.isfile(sys.argv[2]) == True:
		br = BasisReader(sys.argv[1])
		csw = ConvertedSystemWriter(br, sys.argv[2], sys.argv[3])
		found_error = csw.new_system_writer()
		if (found_error > -1):
			print "error: unrecognized identifier in line " + str(found_error + 1)
			sys.exit()

	if os.path.isfile(sys.argv[1]) == False:
		print "error: basis file not found"
	if os.path.isfile(sys.argv[2]) == False:
		print "error: file with equational system not found"



if __name__ == "__main__":
	main()
