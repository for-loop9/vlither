import os
import subprocess

shader_dir = '../app/res/shaders' 

def compile_shader(file_path, output_path):
	try:
		subprocess.run(['glslc', file_path, '-o', output_path], check=True)
		print(f"Compiled {file_path} to {output_path}")
	except subprocess.CalledProcessError as e:
		print(f"Failed to compile {file_path}: {e}")

for filename in os.listdir(shader_dir):
	if filename.endswith('.vert'):
		input_path = os.path.join(shader_dir, filename)
		output_path = os.path.join(shader_dir, filename.replace('.vert', 'v.spv'))
		compile_shader(input_path, output_path)
	elif filename.endswith('.frag'):
		input_path = os.path.join(shader_dir, filename)
		output_path = os.path.join(shader_dir, filename.replace('.frag', 'f.spv'))
		compile_shader(input_path, output_path)

shader_dir = '../ignite/res/shaders'
for filename in os.listdir(shader_dir):
	if filename.endswith('.vert'):
		input_path = os.path.join(shader_dir, filename)
		output_path = os.path.join(shader_dir, filename.replace('.vert', 'v.spv'))
		compile_shader(input_path, output_path)
	elif filename.endswith('.frag'):
		input_path = os.path.join(shader_dir, filename)
		output_path = os.path.join(shader_dir, filename.replace('.frag', 'f.spv'))
		compile_shader(input_path, output_path)