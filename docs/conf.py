import sys
import subprocess

project = 'Reeemake'
copyright = '2020'

html_theme = 'alabaster'

subprocess.call('doxygen', shell=True)

sys.path.append('/home/jaddison/.local/lib/python3.6/site-packages/breathe')

extensions = [
    'breathe'
]

breathe_projects = { 'Reeemake': '/home/jaddison/Documents/reeemake/docs/xml/' }
breathe_default_project = 'Reeemake'