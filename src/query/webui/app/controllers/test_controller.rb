class TestController < ApplicationController
  def index
    foo = { "text" => "123", "children"=> ["text" => "test", "id"=> "36","children" => "foo"]}
    logger.debug "TEST_1"

    respond_to do |format|
	    format.html
      format.json {
        render :json => foo.to_json
      }
    end
  end

  def get_domains
	  logger.debug "TEST"
    @domains= [{ "text" => "123", "expanded"=> true,"children"=> [{"text" => "test", "id"=> "36"},{"text" => "test2", "id"=> "36"}]}, {"text" => "456"}] #{:name =>"2"}
    respond_to do |format|
      format.json { render :json => @domains.to_json}
    end
  end
end
