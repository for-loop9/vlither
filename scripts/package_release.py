import os
import shutil
import subprocess
import argparse

def delete_shader_files(dir):
	for root, dirs, files in os.walk(dir):
		for file in files:
			if file.endswith('.vert') or file.endswith('.frag'):
				file_path = os.path.join(root, file)
				os.remove(file_path)

def main():
	parser = argparse.ArgumentParser(description = "release package")
	parser.add_argument('version', type = str, help = "version number")
	args = parser.parse_args()

	if os.path.isdir('build'):
		shutil.rmtree('build')
		print('deleted build.')

	if os.path.isdir('release'):
		shutil.rmtree('release')
		print('deleted release.')

	subprocess.run(['py', 'scripts/compile_shaders.py'], check = True, text = True)
	subprocess.run(['premake5', '--file=build.lua', 'gmake2'], check = True, text = True)
	subprocess.run(['make', '-C', 'build/makefiles', 'config=release'], check = True, text = True)

	os.makedirs('release', exist_ok = True);

	shutil.copytree('ignite', os.path.join('release', 'ignite'))
	shutil.copytree('app', os.path.join('release', 'app'))

	ignite_src_folder_path = os.path.join('release', 'ignite', 'src')
	app_src_folder_path = os.path.join('release', 'app', 'src')

	shutil.rmtree(ignite_src_folder_path)
	print("deleted 'release/ignite/src'")
	shutil.rmtree(app_src_folder_path)
	print("deleted 'release/ignite/src'")

	delete_shader_files('release/ignite/res/shaders')
	delete_shader_files('release/app/res/shaders')
	print('deleted shader files')

	if os.name == 'nt':
		os.rename("build/bin/app.exe", "build/bin/vlither.exe")
		executable_name = 'vlither.exe'
	else:
		os.rename("build/bin/app", "build/bin/vlither")
		executable_name = 'vlither'

	executable_src = os.path.join('build', 'bin', executable_name)
	executable_dst = os.path.join('release', executable_name)
	license_dst = os.path.join('release', 'LICENSE')

	shutil.copy2(executable_src, executable_dst)
	print('copied executable')

	shutil.copy2('LICENSE', license_dst)
	print('copied license')

	if os.name == 'nt':
		archive_name = f'vlither-{args.version}-win-x86_64'
	else:
		archive_name = f'vlither-{args.version}-linux-x86_64'

	shutil.make_archive(archive_name, 'zip', 'release')

	print(f'created archive {archive_name}.zip')
	print('release packaged successfully')

if __name__ == '__main__':
	main()

shutil.rmtree('release')
shutil.rmtree('build')