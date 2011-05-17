# Configure Rails 3.1 to have assert_select_jquery
module Jquery
  module Rails

    class Engine < ::Rails::Engine
      config.before_configuration do
        require "jquery-rails/assert_select_jquery" if ::Rails.env.test?
      end
    end

  end
end
