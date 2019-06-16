Function dev64 {

# http://evandontje.com/2013/06/06/emulate-the-visual-studio-command-prompt-in-powershell/
# Move to the directory where vcvarsall.bat is stored
#pushd "$env:VS140COMNTOOLS\..\..\VC"
pushd "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools"
# Call the .bat file to set the variables in a temporary cmd session and use 'set' to read out all session variables and pipe them into a foreach to iterate over each variable
cmd /c "VsDevCmd.bat -arch=amd64 -host_arch=amd64 &set" | foreach {
  # if the line is a session variable
  if( $_ -match "=" )
  {
    $pair = $_.split("=");

    # Set the environment variable for the current PowerShell session
    Set-Item -Force -Path "ENV:\$($pair[0])" -Value "$($pair[1])"
  }
}

# Move back to wherever the prompt was previously
popd

}

dev64