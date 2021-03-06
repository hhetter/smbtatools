class OverviewController < ApplicationController

  before_filter :required_config, :check_user
  require "rexml/document"
  include REXML
  def index
  end

  def get_domains
    initial_command
    initialize_domains
    if params[:domain]
      @domain = params[:domain]
    end
    respond_to do |format|
      format.js
    end
  end

  def refresh_domains
    initial_command
    initialize_domains
    @domain = params[:domain]
    respond_to do |format|
      format.js
    end
  end

  def get_shares_and_users
   
    initial_command
    initialize_shares
    initialize_users
     if params[:share] 
      @share = params[:share]
    end
    if params[:user] 
      @user = params[:user]
    end
    respond_to do |format|
      format.js
    end
  end

  def refresh_shares_and_users
    initial_command
    initialize_shares
    initialize_users
    @domain = params[:domain]
    @share = params[:share]
    @user = params[:user]
    respond_to do |format|
      format.js
    end
  end

  def get_files
    @domain = params[:domain]
    @share = params[:share]
    @user = params[:user]
    initial_command
    initialize_files
    if params[:file]
      @file = params[:file]
    end
    respond_to do |format|
      format.js
    end
  end

  def refresh_files
    @domain = params[:domain]
    @share = params[:share]
    @user = params[:user]
    @file = params[:file]
    initial_command
    initialize_files
    respond_to do |format|
      format.js
    end
  end

  def initialize_domains
    initial_command
    cmd = @cmd + " -q 'global, list domains;' -x #{Dir.tmpdir}/domains.xml"
    `#{cmd}`
    @domains = Array.new
    @domains << "(All)"
    @domain = "(All)"
    file = File.new( "#{Dir.tmpdir}/domains.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='domain']") {
      |e| @domains << e.text
    }
    File.delete("#{Dir.tmpdir}/domains.xml")
  end

  def initialize_shares
    @domain = params[:domain]
    initial_command
    if @domain == "(All)"
      cmd = @cmd + " -q 'global, list shares;' -x #{Dir.tmpdir}/shares.xml"
    else
      cmd = @cmd + " -q 'domain " + @domain + ", list shares;' -x #{Dir.tmpdir}/shares.xml"
    end
    `#{cmd}`
    @shares = Array.new
    @shares << "(All)"
    @share = "(All)"
    file = File.new( "#{Dir.tmpdir}/shares.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='sharename']") {
      |e| @shares << e.text
    }
    File.delete("#{Dir.tmpdir}/shares.xml")
  end

  def initialize_users
    @domain = params[:domain]
    initial_command
    if @domain == "(All)"
      cmd = @cmd + " -q 'global, list users;' -x #{Dir.tmpdir}/users.xml"
    else
      cmd = @cmd + " -q 'domain " + @domain + ", list users;' -x #{Dir.tmpdir}/users.xml"
    end
    `#{cmd}`
    @users = Array.new
    @users << "(All)"
    @user = "(All)"
    file = File.new( "#{Dir.tmpdir}/users.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='username']") {
      |e| @users << e.text
    }
    File.delete("#{Dir.tmpdir}/users.xml")
  end

  def initialize_files
    if @domain.empty?
      @domain = "(All)"
    end
    if @share.empty?
      @share = "(All)"
    end
    if @user.empty?
      @user = "(All)"
    end
    @file = params[:file]
    initial_command
    
    if @domain == "(All)"
      if @user == "(All)" and @share == "(All)"
        cmd = @cmd + " -q 'global, list files;' -x #{Dir.tmpdir}/files.xml"
      end
      if @user == "(All)" and @share != "(All)"
        cmd = @cmd + " -q 'global, share " + @share + ", list files;' -x #{Dir.tmpdir}/files.xml"
      end
      if @user != "(All)" and @share == "(All)"
        cmd = @cmd + " -q 'global, user " + @user + ", list files;' -x #{Dir.tmpdir}/files.xml"
      end
      if @user != "(All)" and @share != "(All)"
        cmd = @cmd + " -q 'global, user " + @user + ", share " + @share + ", list files;' -x #{Dir.tmpdir}/files.xml"
      end
    end
    if @domain != "(All)"
      if @user == "(All)" and @share == "(All)"
        cmd = @cmd + " -q 'domain " + @domain + ", list files;' -x #{Dir.tmpdir}/files.xml"
      end
      if @user == "(All)" and @share != "(All)"
        cmd = @cmd + " -q 'domain " + @domain + ", share " + @share + ", list files;' -x #{Dir.tmpdir}/files.xml"
      end
      if @user != "(All)" and @share == "(All)"
        cmd = @cmd + " -q 'domain " + @domain + ", user " + @user + ", list files;' -x #{Dir.tmpdir}/files.xml"
      end
      if @user != "(All)" and @share != "(All)"
        cmd = @cmd + " -q 'domain " + @domain + ", user " + @user + ", share " + @share + ", list files;' -x #{Dir.tmpdir}/files.xml"
      end
    end
    `#{cmd}`
    @files = Array.new
    file = File.new( "#{Dir.tmpdir}/files.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='filename']") {
      |e| @files << e.text
    }
    File.delete("#{Dir.tmpdir}/files.xml")
  end

  def temp_functions
    @all_functions = Array.new
    @functions = Temp.where("user = #{current_user.id}")
    @functions.each do |function|
      @cmd = function.function
      @id = function.id
      create_file_and_divname
    end
    render :layout => false
  end

  def create_file_and_divname
    @temp_function = Array.new
    @cmd += " -o html > #{Dir.tmpdir}/function.html"
    `#{@cmd}`
    @output = File.open("#{Dir.tmpdir}/function.html", "r")
    @output = @output.readlines.to_s
    @output = @output.html_safe
    File.delete("#{Dir.tmpdir}/function.html")
    @divname = Digest::MD5.hexdigest(@cmd)
    @temp_function << @output
    @temp_function << @divname
    @temp_function << @cmd
    @temp_function << @id
    @all_functions << @temp_function
  end

  private

  def required_config
    if session[:dbdriver].blank? || session[:dbname].blank? || session[:dbuser].blank? || session[:dbhost].blank?
      redirect_to config_url, :flash => {:error => "Please check your configuration."}
    end
  end

end
