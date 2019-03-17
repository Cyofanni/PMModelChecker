#accept a pretty-written system and the basis with readable names

import sys
import os

class BasisReader:
	def __init__(self, basis_file):
		#get basis file as input
		self._file_handler = open(basis_file, "r")

	def read(self):
		names_indices = {}
		line_counter = 0
		for line in self._file_handler.readlines():
			line_no_newline = line.replace('\n','')
			names_indices[line_no_newline] = line_counter
			line_counter += 1
		return names_indices    #{"a":0, "b":1 etc...}


class ConvertedSystemWriter:
	def __init__(self, br, system_file, output_file):
		self._basis_reader = br
		self._system_file = open(system_file, "r")
		self._output_file = open(output_file, "w")

	def new_system_generator_writer(self):
		basis_names_indices = self._basis_reader.read()
		for line in self._system_file.readlines():
			if line[0:3] == "phi":
                		converted_line = ""
				#handle 'phi' identifier
				first_underscore_position = 3
				second_underscore_position = line.find('_', 4)
				basis_identifier = line[first_underscore_position+1 : second_underscore_position]
				basis_index = basis_names_indices[basis_identifier]

				line = line[0:first_underscore_position+1] + str(basis_index) + line[second_underscore_position:]

				#end of 'phi' identifier processing
				#now process atoms at the rhs
				find_from = line.find('=')  #start looking for '[' after the '=' sign
				while line.find('[', find_from+1) != -1:
					find_from = line.find('[', find_from+1)
					next_comma_position = line.find(',', find_from)
					basis_identifier = line[find_from+1 : next_comma_position]
					basis_index = basis_names_indices[basis_identifier]

					line = line[0:find_from + 1] + "b_" + str(basis_index) + line[next_comma_position:]

			self._output_file.write(line)




def main():
	if (len(sys.argv) < 4):
		print "usage: python basis_converter.py [basis_file] [system_file] [output_file]"
		sys.exit()
	br = BasisReader(sys.argv[1])
	csw = ConvertedSystemWriter(br, sys.argv[2], sys.argv[3])
	csw.new_system_generator_writer()




if __name__ == "__main__":
	main()
