class FunctionsController < ApplicationController
  require "digest/md5"
  require "rexml/document"
  include REXML
  before_filter :check_user

  def start_function
   
    if params[:func] == "smbtad-report" || params[:func] == "self-check"
      get_system_info
    else
      get_objects
      get_time_modifiers
      get_function
    end
    create_file_and_divname
    render :update do |page|
      page.insert_html :after, "list", :partial => "start_function"
    end
  end

  def global_search
    initial_command
    search
    render :layout => false
  end

  def search
    @search_string = "%" + params[:q] + "%"
    @cmd += "-q \"global, search " + @search_string + ";\""
    @cmd = @cmd + " -x #{Dir.tmpdir}/compare.xml"

    `#{@cmd}`
    @output = File.open("#{Dir.tmpdir}/compare.xml", "r")
    file = File.new( "#{Dir.tmpdir}/compare.xml" )
    doc = Document.new file
    @search_result_domains = Array.new
    @search_result_shares = Array.new
    @search_result_users = Array.new
    @search_result_files = Array.new
    @buffer_search_result = Array.new
    doc.elements.each("smbta_output/search/result"){

      |object| object.elements.each("domain/name"){
        |domain| @search_result_domains << domain.text
      }
     
      object.elements.each("share"){
        |all_shares| all_shares.elements.each("name"){
          |share| @buffer_search_result << share.text
        }
        all_shares.elements.each("domain"){
          |domain| @buffer_search_result << domain.text
        }
        @search_result_shares << @buffer_search_result
        @buffer_search_result = []
      }
      object.elements.each("user"){
        |all_users| all_users.elements.each("name"){
          |user| @buffer_search_result << user.text
        }
        all_users.elements.each("domain"){
          |domain| @buffer_search_result << domain.text
        }
        @search_result_users << @buffer_search_result
        @buffer_search_result =[]
      }
      object.elements.each("file"){
        |all_files| all_files.elements.each("name"){
          |f| @buffer_search_result << f.text
        }
        all_files.elements.each("share"){
          |share| @buffer_search_result << share.text
        }
        all_files.elements.each("domain"){
          |domain| @buffer_search_result << domain.text
        }
        @search_result_files << @buffer_search_result
        @buffer_search_result =[]
      }
    }
    File.delete("#{Dir.tmpdir}/compare.xml")
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
    if @function == "total" || @function == "24h_usage"
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

  def get_system_info
    initial_command
    @info = params[:func]
    if @info == "smbtad-report"
      @report = params[:report]
      @cmd += "-q \"system, " + @info + " " + @report + ";\""
    end
    if @info == "self-check"
      @self_check = params[:self_check]
      @cmd += "-q \"system, " + @info + " " + @self_check + ";\""
    end
    create_file_and_divname
  end

  def create_file_and_divname
    @cmd += " -o html > #{Dir.tmpdir}/function.html"
    `#{@cmd}`
    @output = File.open("#{Dir.tmpdir}/function.html", "r")
    @output = @output.readlines.to_s
    @output = @output.html_safe
    File.delete("#{Dir.tmpdir}/function.html")
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
    @divname << @report
    @divname << @self_check
    @divname = @divname.to_s
    @divname = Digest::MD5.hexdigest(@divname)
  end

  def printview
    @cmd = params[:cmd]
    `#{@cmd}`
    @output = File.open("#{Dir.tmpdir}/function.html", "r")
    @output = @output.readlines.to_s
    @output = @output.html_safe
    File.delete("#{Dir.tmpdir}/function.html")
  end

  def save_function
    @cmd = params[:cmd]
    @cmd.chomp!(" -o html > #{Dir.tmpdir}/function.html")
    output = %x[#{@cmd}]
    send_data(output, :filename => "function-" + Time.now.strftime("%Y-%m-%d-%H%M%S").to_s + ".txt",
      :type => 'text/plain')
  end

  def refresh_function
    @cmd = params[:cmd]
    @divname = params[:divname]
     `#{@cmd}`
    @output = File.open("#{Dir.tmpdir}/function.html", "r")
    @output = @output.readlines.to_s
    @output = @output.html_safe
    File.delete("#{Dir.tmpdir}/function.html")
    render :update do |page|
      page.replace @divname, :partial => "start_function"
    end
  end

end
