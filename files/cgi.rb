class ResponseMsg
	@@status_msg = {200 => "OK", 500 => "Internal Server Error"}

	attr_writer :status, :body
	def initialize()
		@status = 200
		@body = ""
	end

	def construct_msg()
		response_header = "#{ENV["SERVER_PROTOCOL"]} #{@status} #{@@status_msg[@status]}\r\n"
		if @body.length > 0
			response_header += "Content-Length: #{@body.length}\r\n"
		end
		response_header += "\r\n"
		return response_header + @body
	end
end

class Cgi
	def initialize(request_body)
		@path_info		= ENV["PATH_INFO"][0] == '/' ? ENV["PATH_INFO"][1..-1] : ENV["PATH_INFO"]
		@response_msg = ResponseMsg.new()
		@request_body = request_body
		@query				= Hash.new()
		if ENV["QUERY_STRING"]
			split = ENV["QUERY_STRING"].split("&")
			split.each do |s|
				split2 = s.split('=')
				@query[split2[0]] = split2[1]
			end
		end
	end

	def execute_get()
		name = @query["name"] ? @query["name"] : "Default name"
		nickname = @query["nickname"] ? @query["nickname"] : "Default nickname"

		@response_msg.status = 200
		@response_msg.body = "Name: #{name}\nNickname: #{nickname}"

		print @response_msg.construct_msg()
	end

	def execute_post()
		@response_msg.status = 200
		if @query["type"] == "reverse"
			@response_msg.body = @request_body.reverse
		elsif @query["type"] == "upcase"
			@response_msg.body = @request_body.upcase
		else
			@response_msg.body = @request_body.downcase
		end
		print @response_msg.construct_msg()
	end

	def execute_put()

		begin
			raise "Content-Length is required for PUT request to CGI" unless ENV["CONTENT_LENGTH"]
			timenow = Time.now.to_i
			if @path_info.empty?
				outfile = File.new(timenow.to_s, "w+")
			else
				outfile = File.new(@path_info, "w+")
			end

			outfile.print(@request_body)
			outfile.close

			@response_msg.status = 200
			@response_msg.body = "Check ./#{@path_info.empty? ? timenow : @path_info} if its has size of #{ENV["CONTENT_LENGTH"]}."
			print @response_msg.construct_msg()

		rescue Exception => e
			@response_msg.status = 500
			@response_msg.body = e.message
			print @response_msg.construct_msg()
		end
	end

	def execute_delete()
		begin
			raise "No specified PATH_INFO" unless !@path_info.empty?

			File.delete(@path_info)

			@response_msg.status = 200
			@response_msg.body = "Successfuly deleted #{@path_info}"
			print @response_msg.construct_msg()
		rescue Exception => e
			@response_msg.status = 500
			@response_msg.body = e.message
			print @response_msg.construct_msg()
		end
	end
end


if __FILE__ == $PROGRAM_NAME
	if !ENV["REQUEST_METHOD"] || !ENV["SERVER_PROTOCOL"] || !ENV["PATH_INFO"] || !ENV["SERVER_NAME"] || !ENV["SERVER_PORT"]
		body = "#{$PROGRAM_NAME} Requires REQUEST_METHOD, SERVER_PROTOCOL, PATH_INFO, SERVER_NAME and SERVER_PORT\n"
		content_length = body.length
		print "HTTP/1.1 500 Internal Server Error\r\nContent-Length: #{content_length}\r\n\r\n#{body}"
		exit 1
	end

	body = STDIN.gets
	cgi = Cgi.new(body)
	method = ENV["REQUEST_METHOD"]
	if method == "GET"
		cgi.execute_get()
	elsif method == "POST"
		cgi.execute_post()
	elsif method == "PUT"
		cgi.execute_put()
	elsif method == "DELETE"
		cgi.execute_delete()
	end
end
