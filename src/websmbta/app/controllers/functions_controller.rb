class FunctionsController < ApplicationController
  require "digest/md5"
  before_filter :check_user

  def start_function
    get_objects
    get_time_modifiers
    get_function
    creat_file_and_divname
    render :update do |page|
      page.insert_html :after, "list", :partial => "start_function"
    end
  end

  def get_objects
    initial_command
    @domain = params[:domain]
    @share = params[:share]
    @user = params[:user]
    @file = params[:file]
    if @domain == ""
      @domain = "(All)"
    end
    if @share == ""
      @share = "(All)"
    end
    if @user == ""
      @user = "(All)"
    end
    if @domain == "(All)"
      if @user == "(All)" and @share == "(All)" and @file.blank?
        @cmd += " -q \"global"
      end
      if @user == "(All)" and @share == "(All)" and !@file.blank?
        @cmd += "-q \"global, file " + @file
      end
      if @user == "(All)" and @share != "(All)" and @file.blank?
        @cmd += "-q \"global, share " + @share
      end
      if @user == "(All)" and @share != "(All)" and !@file.blank?
        @cmd += "-q \"global, share " + @share + ", file " + @file
      end
      if @user != "(All)" and @share == "(All)" and @file.blank?
        @cmd += "-q \"global, user " + @user
      end
      if @user != "(All)" and @share == "(All)" and !@file.blank?
        @cmd += "-q \"global, user " + @user + ", file " + @file
      end
      if @user != "(All)" and @share != "(All)" and @file.blank?
        @cmd += "-q \"global, user " + @user + ", share " + @share
      end
      if @user != "(All)" and @share != "(All)" and !@file.blank?
        @cmd += "-q \"global, user " + @user + ", share " + @share + ", file " + @file
      end
    end
    if @domain != "(All)"
      if @user == "(All)" and @share == "(All)" and @file.blank?
        @cmd += " -q \"domain " + @domain
      end
      if @user == "(All)" and @share == "(All)" and !@file.blank?
        @cmd += "-q \"domain " + @domain + ", file " + @file
      end
      if @user == "(All)" and @share != "(All)" and @file.blank?
        @cmd += "-q \"domain " + @domain + ", share " + @share
      end
      if @user == "(All)" and @share != "(All)" and !@file.blank?
        @cmd += "-q \"domain " + @domain + ", share " + @share + ", file " + @file
      end
      if @user != "(All)" and @share == "(All)" and @file.blank?
        @cmd += "-q \"domain " + @domain + ", user " + @user
      end
      if @user != "(All)" and @share == "(All)" and !@file.blank?
        @cmd += "-q \"domain " + @domain + ", user " + @user + ", file " + @file
      end
      if @user != "(All)" and @share != "(All)" and @file.blank?
        @cmd += "-q \"domain " + @domain + ", user " + @user + ", share " + @share
      end
      if @user != "(All)" and @share != "(All)" and !@file.blank?
        @cmd += "-q \"domain " + @domain + ", user " + @user + ", share " + @share + ", file " + @file
      end
    end
  end

  def get_time_modifiers
    @from = params[:from]
    @to = params[:to]
    if @from == "" and @to == ""
      @cmd += ", "
    end
    if @from != "" and @to == ""
      @cmd += " since \'" + @from + "\', "
    end
    if @from != "" and @to != ""
      @cmd += " from \'" + @from + "\' to \'" + @to + "\', "
    end
  end

  def get_function
    @search = params[:search]
    @function = params[:func]
    @mode = params[:mode]
    @object = params[:object]
    @number = params[:number]
    @timemode = params[:timemode]
    if params[:sort] == "descending"
      @sort = "desc"
    end
    if params[:sort] == "ascending"
      @sort = "asc"
    end
    @cmd += @function
    if @function == "total" || @function == "usage"
      @cmd += " " + @mode + ";\""
    end
    if @function == "list"
      @cmd += " " + @object + ";\""
    end
    if @function == "top"
      @cmd += " " + @number + " " + @object + " " + @mode +  " " + @sort + ";\""
    end
    if @function == "last_activity"
      @cmd += " " + @number + ";\""
    end
    if @function == "search"
      @cmd += " " + @search + ";\""
    end
    if @function == "throughput"
      @cmd += " " + @number + " " + @timemode + " " + @mode + ";\""
    end
  end

  def creat_file_and_divname
    @cmd += " -o html > /tmp/function.html"
    `#{@cmd}`
    @output = File.open("/tmp/function.html", "r")
    @output = @output.readlines.to_s
    @output = @output.html_safe
    File.delete("/tmp/function.html")
    @divname = Array.new
    @divname << @function
    @divname << @domain
    @divname << @share
    @divname << @user
    @divname << @file
    @divname << @search
    @divname << @mode
    @divname << @object
    @divname << @number
    @divname << @sort
    @divname = @divname.to_s
    @divname = Digest::MD5.hexdigest(@divname)
  end

  def printview
    @cmd = params[:cmd]
    `#{@cmd}`
    @output = File.open("/tmp/function.html", "r")
    @output = @output.readlines.to_s
    @output = @output.html_safe
    File.delete("/tmp/function.html")
  end

  def save_function
    @cmd = params[:cmd]
    @cmd.chomp!(" -o html > /tmp/function.html")
    output = %x[#{@cmd}]
    send_data(output, :filename => "function-" + Time.now.strftime("%Y-%m-%d-%H%M%S").to_s + ".txt",
      :type => 'text/plain')
  end

  def refresh_function
    @cmd = params[:cmd]
    @divname = params[:divname]
     `#{@cmd}`
    @output = File.open("/tmp/function.html", "r")
    @output = @output.readlines.to_s
    @output = @output.html_safe
    File.delete("/tmp/function.html")
    render :update do |page|
      page.replace @divname, :partial => "start_function"
    end
  end

end
