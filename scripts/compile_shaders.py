import os
import subprocess

def compile_shader(file_path, output_path):
	try:
		subprocess.run(['glslc', file_path, '-o', output_path], check=True)
		print(f"Compiled {file_path} to {output_path}")
	except subprocess.CalledProcessError as e:
		print(f"Failed to compile {file_path}: {e}")

def compile_dir(dir):
	for filename in os.listdir(dir):
		if filename.endswith('.vert'):
			input_path = os.path.join(dir, filename)
			output_path = os.path.join(dir, filename.replace('.vert', 'v.spv'))
			compile_shader(input_path, output_path)
		elif filename.endswith('.frag'):
			input_path = os.path.join(dir, filename)
			output_path = os.path.join(dir, filename.replace('.frag', 'f.spv'))
			compile_shader(input_path, output_path)

compile_dir('ignite/res/shaders')
compile_dir('app/res/shaders')