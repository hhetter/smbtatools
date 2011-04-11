class ApplicationController < ActionController::Base
  protect_from_forgery
  def initial_command
    if $type == "Inet-Port"
      @cmd="smbtaquery -h " + $host_ip + " -i " + $port + " -I 0 "
    end
    if $type == "Unix Domain Socket"
      @cmd="smbtaquery -h " + $host_ip + " -u " + $port + " -I 0 "
    end
  end
end
