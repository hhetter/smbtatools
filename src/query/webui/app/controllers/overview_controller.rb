class OverviewController < ApplicationController
  before_filter :required_config
  require "rexml/document"
  include REXML
  def index
  end

  def get_domains
    @domain = params[:domain]
    initial_command
    initialize_domains
    render :update do |page|
      page << "if (!$('div#domains').length)"
      page.insert_html :after, "global", :partial => "domains"
      page << "if($('div#domains').length)"
      page.replace "domains", :partial => "domains"
      page.call "refreshDomains"
    end
    end
  

  def refresh_domains
    initial_command
    initialize_domains
    @domain = params[:domain]
    render :update do |page|
      page.replace "domains", :partial => "domains"
    end
  end

  def get_shares_and_users
    initial_command
    initialize_shares
    initialize_users
    @domain = params[:domain]
    render :update do |page|
      page << "if (!$('div#shares').length)"
      page.insert_html :after, "domains", :partial => "shares"     
      page << "if($('div#shares').length)"
      page.replace "shares", :partial => "shares"
      page.delay(1) do
        page << "if (!$('div#users').length)"
        page.insert_html :bottom, "shares", :partial => "users"
        page << "if($('div#users').length)"
        page.replace "users", :partial => "users"
      end
      page.call "refreshShares"
    end
  end

  def refresh_shares_and_users
    initial_command
    initialize_shares
    initialize_users
    @domain = params[:domain]
    @share = params[:share]
    @user = params[:user]
    render :update do |page|
      page.replace "shares", :partial => "shares"
      page.insert_html :bottom, "shares", :partial => "users"
    end
  end

  def get_files
    @domain = params[:domain]
    @share = params[:share]
    @user = params[:user]
    initial_command
    initialize_files
    render :update do |page|
      page << "if (!$('div#files').length)"
      page.insert_html :after, "shares", :partial => "files"
      page << "if($('div#files').length)"
      page.replace "files", :partial => "files"
      page.call "refreshFiles"
    end
  end

  def refresh_files
    @domain = params[:domain]
    @share = params[:share]
    @user = params[:user]
    @file = params[:file]
    initial_command
    initialize_files
    render :update do |page|
      page.replace "files", :partial => "files"
    end
  end

  def initialize_domains
    initial_command
    cmd=@cmd + " -q 'global, list domains;' -x /tmp/domains.xml"
    `#{cmd}`
    @domains = Array.new
    file = File.new( "/tmp/domains.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='domain']") {
      |e| @domains << e.text
    }
    File.delete("/tmp/domains.xml")
  end

  def initialize_shares
    @domain = params[:domain]
    initial_command
    if @domain.blank?
      cmd= @cmd + " -q 'global, list shares;' -x /tmp/shares.xml"
    else
      cmd= @cmd + " -q 'domain " + @domain + ", list shares;' -x /tmp/shares.xml"
    end
    `#{cmd}`
    @shares = Array.new
    file = File.new( "/tmp/shares.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='sharename']") {
      |e| @shares << e.text
    }
    File.delete("/tmp/shares.xml")
  end

  def initialize_users
    @domain = params[:domain]
    initial_command
    if @domain.blank?
      cmd= @cmd + " -q 'global, list users;' -x /tmp/users.xml"
    else
      cmd= @cmd + " -q 'domain " + @domain + ", list users;' -x /tmp/users.xml"
    end
    `#{cmd}`
    @users = Array.new
    file = File.new( "/tmp/users.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='username']") {
      |e| @users << e.text
    }
    File.delete("/tmp/users.xml")
  end

  def initialize_files
    @domain = params[:domain]
    @share = params[:share]
    @user = params[:user]
    initial_command
    if @domain.blank?
      if @user.blank? and @share.blank?
        cmd = @cmd + " -q 'global list files;' -x /tmp/files.xml"
      end
      if @user.blank? and ! @share.blank?
        cmd = @cmd + " -q 'global, share " + @share + ", list files;' -x /tmp/files.xml"
      end
      if !@user.blank? and @share.blank?
        cmd = @cmd + " -q 'global, user " + @user + ", list files;' -x /tmp/files.xml"
      end
      if !@user.blank? and !@share.blank?
        cmd = @cmd + " -q 'global, user " + @user + " share " + @share + ", list files;' -x /tmp/files.xml"
      end
    end
    if !@domain.blank?
      if @user.blank? and @share.blank?
        cmd = @cmd + " -q 'domain " + @domain + ", list files;' -x /tmp/files.xml"
      end
      if @user.blank? and ! @share.blank?
        cmd = @cmd + " -q 'domain " + @domain + ", share " + @share + ", list files;' -x /tmp/files.xml"
      end
      if !@user.blank? and @share.blank?
        cmd = @cmd + " -q 'domain " + @domain + ", user " + @user + ", list files;' -x /tmp/files.xml"
      end
      if !@user.blank? and !@share.blank?
        cmd = @cmd + " -q 'domain " + @domain + ", user " + @user + ", share " + @share + ", list files;' -x /tmp/files.xml"
      end
    end
    `#{cmd}`
    @files = Array.new
    file = File.new( "/tmp/files.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='filename']") {
      |e| @files << e.text
    }
    File.delete("/tmp/files.xml")
  end

  def initial_command
    if $type == "Inet-Port"
      @cmd="smbtaquery -h " + $host_ip + " -i " + $port + " -I 0 "
    end
    if $type == "Unix Domain Socket"
      @cmd="smbtaquery -h " + $host_ip + " -u " + $port + " -I 0 "
    end
  end

  private

  def required_config
    if $host_ip.blank? || $port.blank?
      redirect_to :root, :flash => {:error => "Please enter an valid IP and Port"}
    end
  end

end
