import requests

"""
    getExploits():
        Get exploits
"""
def getExploits():
    # Get description of the database
    exploitdb_description_url   = "https://raw.githubusercontent.com/offensive-security/exploitdb/master/files_exploits.csv"
    exploitdb_description_file  = requests.get(exploitdb_description_url).content
    exploitdb_description_lines = exploitdb_description_file.decode().split('\n')

    # Get categories
    categories = exploitdb_description_lines[0].split(',')

    # Parse the content to an array
    exploits = []
    for i in range(1, len(exploitdb_description_lines)):
        fields = exploitdb_description_lines[i].split(',')
        data   = {}
        for j in range(len(fields)):
            data[categories[j]] = fields[j]
        exploits.append(data)

    return exploits


