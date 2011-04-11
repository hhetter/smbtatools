class FunctionsController < ApplicationController
  def start_function
    get_function
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
    if @domain.blank?
      if @user.blank? and @share.blank? and @file.blank?
        @cmd += " -q 'global, "
      end
      if @user.blank? and @share.blank? and !@file.blank?
        @cmd += "-q 'global, file " + @file + ", "
      end
      if @user.blank? and !@share.blank? and @file.blank?
        @cmd += "-q 'global, share " + @share + ", "
      end
      if @user.blank? and !@share.blank? and !@file.blank?
        @cmd += "-q 'global, share " + @share + ", file " + @file + ", "
      end
      if !@user.blank? and @share.blank? and @file.blank?
        @cmd += "-q 'global, user " + @user + ", "
      end
      if !@user.blank? and @share.blank? and !@file.blank?
        @cmd += "-q 'global, user " + @user + ", file " + @file + ", "
      end
      if !@user.blank? and !@share.blank? and @file.blank?
        @cmd += "-q 'global, user " + @user + ", share " + @share + ", "
      end
      if !@user.blank? and !@share.blank? and !@file.blank?
        @cmd += "-q 'global, user " + @user + ", share " + @share + ", file " + @file + ", "
      end
    end
    if !@domain.blank?
      if @user.blank? and @share.blank? and @file.blank?
        @cmd += " -q 'domain " + @domain + ", "
      end
      if @user.blank? and @share.blank? and !@file.blank?
        @cmd += "-q 'domain " + @domain + ", file " + @file + ", "
      end
      if @user.blank? and !@share.blank? and @file.blank?
        @cmd += "-q 'domain " + @domain + ", share " + @share + ", "
      end
      if @user.blank? and !@share.blank? and !@file.blank?
        @cmd += "-q 'domain " + @domain + ", share " + @share + ", file " + @file + ", "
      end
      if !@user.blank? and @share.blank? and @file.blank?
        @cmd += "-q 'domain " + @domain + ", user " + @user + ", "
      end
      if !@user.blank? and @share.blank? and !@file.blank?
        @cmd += "-q 'domain " + @domain + ", user " + @user + ", file " + @file + ", "
      end
      if !@user.blank? and !@share.blank? and @file.blank?
        @cmd += "-q 'domain " + @domain + ", user " + @user + ", share " + @share + ", "
      end
      if !@user.blank? and !@share.blank? and !@file.blank?
        @cmd += "-q 'domain " + @domain + ", user " + @user + ", share " + @share + ", file " + @file + ", "
      end
    end
  end

  def get_function
    get_objects
    @function = params[:func]
    @mode = params[:mode]
    @object = params[:object]
    @number = params[:number]
    @cmd += @function
    if @function == "total" || @function == "usage"
      @cmd += " " + @mode + ";'"
    end
    if @function == "top"
      @cmd += " " + @number + " " + @object + " " + @mode +  ";'"
    end
    if @function == "last_activity"
      @cmd += " " + @number + ";'"
    end
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
    @divname << @mode
    @divname << @object
    @divname << @number
  end
end
