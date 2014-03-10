var warning_msg = '<div class="alert alert-danger" style="display:none">\
<button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>\
<strong>Error!</strong> \"';
var msg_end = '\" is wrong format for this field. </div>';

function check_format(obj,exp) {
	if (obj.val().length>0 && !exp.test(obj.val())) {
		obj.parents('.panel-body').prepend(warning_msg+obj.val()+msg_end);
		obj.parents('.panel-body').children('.alert').slideDown(200);
		obj.val('');
		obj.focus();
		obj.addClass('check-error');
	}
	else
	{
		obj.removeClass('check-error');
		obj.parents('.panel-body').children('.alert').slideUp(200);
		setTimeout(function() {
			obj.parents('.panel-body').children('.alert').remove();
		}, 500);
	}
}

function makepopover(obj) {
	obj.popover({
		content:'Can not be empty.',
		placement:'top',
		trigger:'manual'
	});
	obj.popover('show');
	$('.popover').hover(function(){
		$(this).fadeOut(100);
	});
}

function ParseMission(data) {
	if (data.obj == undefined)
		return;
	var o = {};
	var a = data.obj.find('input,select');
	if (a.length > 0) {
		o['mission-type'] = mission_type[data.obj[0].name];
		a.each(function(){
			if (this.tagName == 'INPUT') {
				switch (this.type) {
					case 'radio':
						if (!$(this).parent().hasClass('active'))
							return;
						o[$(this).parents('.btn-group').attr('name')] = $(this).parent().text().trim().toLowerCase();
						break;
					case 'text':
						if ($(this).parent().css('display') != 'none') {
							if ($(this).val().length > 0)
								o[this.name] = $(this).val();
							else {
								$(this).focus();
								if (!data.auto)
									makepopover($(this));
								data.ok = false;
							}
						}

						break;
					default:
						break;
				}
			} else if (this.tagName == 'SELECT') {
				o[this.name] = $(this).val();
			}
		});
	}

	if ($.isEmptyObject(o))
		return '';
	else
		return JSON.stringify(o);
}

var env_setted = false;
function config_env(type) {
	if (type != config_type.ENV && env_setted)
		return true;
	var host_obj = $('#host-panel .ip-input');
	var target_obj_1 = $('#target-panel-1 .ip-input');
	var target_obj_2 = $('#target-panel-2 .ip-input');
	if (host_obj.val().length <= 0 || host_obj.hasClass('check-error') ||
		((target_obj_1.val().length <=0 || target_obj_1.hasClass('check-error')) &&
		(target_obj_2.val().length <=0 || target_obj_2.hasClass('check-error')))) { 
		show_alert(false,'Env not properly');
		if (!$('#auto-save-checkbox').bootstrapSwitch('status'))
			$("#def-env").trigger("click");
		return false;
	}
	var data = JSON.stringify({'host':$('#host-panel .ip-input').val(),
							'target1':$('#target-panel-1 .ip-input').val(),
							'target2':$('#target-panel-2 .ip-input').val()});
	var type = config_type.ENV;
	senddata(data,type);
}

var STATUS = {
	'RUNNING' : 0,
	'PAUSE' : 1,
	'STOP' : 2,
	'ERROR' : 3
}

var ORDER = {
	'START' : 0,
	'PAUSE' : 1,
	'RESUME' : 2,
	'STOP' : 3
}

function ParseRunner(data) {
	if (data.obj == undefined)
		return

	var r = {};
	var _run = data.obj.hasClass('running_committing');
	var _pause = data.obj.hasClass('pause_committing');
	var _stop = data.obj.hasClass('stop_committing');
	r['type'] = mission_type[data.obj.text()];
	if (_run) {
		data.obj.removeClass('running_committing');
		r['order'] = ORDER.START;
	}
	else if (_pause) {
		data.obj.removeClass('pause-committing');
		r['order'] = ORDER.PAUSE;
	}
	else if (_stop) {
		data.obj.removeClass('stop-committing');
		r['order'] = ORDER.STOP;
	}

	if ($.isEmptyObject(r))
		return ''
	else 
		return JSON.stringify(r);

}

function process_ret(err_msg,type) {
	var ok = true
	if (err_msg != 'OK')
		ok = false;
	else if (type == config_type.ENV)
		env_setted = true;

	show_alert(ok,err_msg);
}

function save_config(auto) {
 	var type = config_type[$('.present').children('span')[0].id];
 	if (type == config_type.RUNNER && auto)
 		return;
 	if (!config_env(type))
 		return;

	var data = {
		'ok':true,
		'auto':auto
	};

	var r;
	if (type == config_type.MISSION) {
		data['obj'] = $('.present .sub-present');
		r = ParseMission(data);
	}
	else if (type == config_type.RUNNER) {
		data['obj'] = $('.mission-id');
		r = ParseRunner(data);
	} else if (type == config_type.ENV)
		return;
		
	var ok = data.ok;
	if (ok) {
		if (r.length > 0) {
			senddata(r,type);
			if (type == config_type.MISSION) {
				add_mission($('.present .sub-present')[0].name);
			}
		} else {
			// no data send
		}
	}
	else if (!auto) // when auto, no alert
		show_alert(ok);
}

function heredoc(fn) {
	return fn.toString().split('\n').slice(1,-1).join('\n')+'\n';
}

function add_mission(name) {
	if ($('.table-empty').length > 0)
		$('.table-empty').remove();
	$('#runner-content .mission-id').each(function(){
		if ($(this).text()==name) {
			$(this).parent().remove();
			return false;
		}			
	});
	var mission_html = heredoc(function(){
		/*!
		<tr>
			<td class="mission-id">12345678</td>
			<td class="progress-td">
				<div class="progress progress-striped">
					<div class="progress-bar" role="progressbar" aria-valuenow="45" aria-valuemin="0" aria-valuemax="100" style="width: 0%" data-percentage="100">
						0%
					</div>
				</div>
			</td>
			<td>
				<button class="btn btn-xs btn-play btn-success">
					<span class="glyphicon glyphicon-play"></span>
				</button>
				<button class="btn btn-xs btn-pause disabled">
					<span class="glyphicon glyphicon-pause"></span>
				</button>
				<button class="btn btn-xs btn-stop disabled">
					<span class="glyphicon glyphicon-stop"></span>
				</button>
			</td>
		</tr>
		*/
	});
	mission_html = mission_html.replace('12345678',name);
	$('#runner-content tbody').append(mission_html);
	BindProgressEvent();
}

function senddata(d,type) {
	var type_url = '/'
	switch(type)
	{
	case config_type.ENV:
		type_url += 'env';
		break;
	case config_type.MISSION:
		type_url += 'mission';
		break;
	case config_type.RUNNER:
		type_url += 'runner';
		break;
	case config_type.SCAN:
		type_url += 'scan';
		break;
	default:
		break;
	}
	var url = (domain.length > 0) ?
		'http://' + domain + type_url : type_url;
	$.ajax({
		type: "POST",
		url: url,
		data: d,
		async: true,
		success: function(data){
			process_ret(data,type);
		}
	});
}
var domain = '192.168.1.106';
var auto_save_interval = 30000;
var mission_type = {
	'ARP_REQUEST_STORM' : 0,
	'ARP_HOST_REPLY_STORM' : 1,
	'ARP_CACHE_SATURATION_STORM' : 2,
	'ARP_GRAMMEAR ' : 3
}

var config_type = {
	'ENV' : 0,
	'SCAN' : 1,
	'MISSION' : 2,
	'RUNNER' : 3
}

var setup_type = {
	'INIT' : 0,
	'OTHER' : 1
}

function show_alert(success,msg)
{
	if (success)
		$('#config-saved-alert-success').slideDown(200).delay(2000).slideUp(200);
	else {
		if (msg != undefined) {
			$('#config-saved-alert-error').html(msg);
		} else {
			$('#config-saved-alert-error').html('Config Save Error !');
		}
		$('#config-saved-alert-error').slideDown(200).delay(2000).slideUp(200);
	}
}

function auto_save_config() {
	if ($('#auto-save-checkbox').bootstrapSwitch('status')) {
		save_config(true);
	} 
}

function destroy_popover() {
	if ($(this).next('.popover').length > 0)
		$(this).popover('destroy');
}

var websocket;
var ctx;
function init_mission() {
	var url = 'ws://'+domain+'/setup';
  	websocket = new WebSocket(url);
  	var writeToScreen = function(message) {
	    $('.modal-body').append('<p>'+message+'</p>');
	};

	var msg = JSON.stringify({'setup_type':setup_type.INIT});
	websocket.onopen = function(ev) {
      writeToScreen('CONNECTED.');
      websocket.send(msg);
      writeToScreen('SEND: '+msg);
    };

    websocket.onerror = function(ev) {
      writeToScreen('<span style="color: red; ">ERROR: </span> ' + ev.data);
    };

    websocket.onmessage = function(ev) {
    	if (!ev.data) return;  // No data, this is a PING message,ignore it
      	writeToScreen('<span style="color: blue;">RESPONSE: ' + ev.data +
                    ' </span>');

      	if (ev.data == '0') {// no mission
      		writeToScreen('NO Mission.');
			setTimeout(function(){
				$('#myModal').modal('hide');
			},2000);
      	}

      	var jsonarray = $.parseJSON(ev.data);
  		$.each(jsonarray,function(i,n){
  			f_y = parseFloat(n.content);
  			if (f_y == NaN) {// fail to float, it an error message 
  				alert(n.content)
  				showChart(ctx,n.seq,0);
  			}
  			showChart(ctx,n.seq,f_y);
  		});
    };
}

$(function(){
	$('.ip-input').blur(function(){
		var exp = /^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[0-9]{1,2})(\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[0-9]{1,2})){3}$/;
		check_format($(this),exp);
	});

	$('.mac-input').blur(function(){
		var exp = /^([A-Fa-f\d]{2})(:([A-Fa-f\d]{2})){5}$"/;
		check_format($(this),exp);
	});

	$('.int-input').keyup(function(){
		$(this).val($(this).val().replace(/\D|^0/g,''));
  	}).bind("paste",function(){ // Ctrl+v event
  		$(this).val($(this).val().replace(/\D|^0/g,''));
  	}).css("ime-mode","disabled"); // disable ime

  	$('.double-input').keyup(function(){
  		$(this).val($(this).val().replace(/[^0-9.]/g,''));
  	}).bind("paste",function(){ // Ctrl+v event
  		$(this).val($(this).val().replace(/[^0-9.]/g,''));
  	}).css("ime-mode","disabled"); // disable ime

  	$('#manual-save-button').on('click',function(){
  		save_config(false);
  	});

  	setInterval(auto_save_config,auto_save_interval);

  	$(window).bind('beforeunload',function(){
    	websocket.onclose = function(){};
    	websocket.close();
    });
});
