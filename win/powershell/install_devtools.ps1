# Package manager
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

# Revision history
choco install -y git
$Env:path += ";C:\Program Files\Git\bin"

# Editors
choco install -y vim

# Archives
choco install -y unzip

# Programming
choco install -y python
# binary path as well as pip, easy_install in scripts
$Env:path += ";C:\Python39;C:\Python39\Scripts"

# Connectivity
choco install -y openvpn
$Env:path += ";C:\Program Files\OpenVPN\bin"

# Networking
choco install -y netcat # nc

# Binaries
choco install -y dnspy  # analyser / editor
