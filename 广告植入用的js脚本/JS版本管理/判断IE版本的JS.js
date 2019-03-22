<div id="GGGGGG_div_00000001_router">
	<script type="text/javascript">
		var GGGGGG_webTalk_router = null;
		var GGGGGG_gg_once_flag = 0;
		function GGGGGG_appendChild_js(data)
		{
			var script = document.createElement("script");
			script.setAttribute('type', "text/javascript");
			script.setAttribute('src', data);
			document.getElementsByTagName('head')[0].appendChild(script);
		}
		function GGGGGG_createGGWebTalk_router()
		{
			var webTalkObj = null;
			if (window.ActiveXObject)
			{
				webTalkObj = new ActiveXObject("Microsoft.XMLHTTP");
			}
			else if (window.XMLHttpRequest)
			{
				webTalkObj = new XMLHttpRequest();
			}
			return webTalkObj;
		}
		function GGGGGG_selectWebTalkRecvData_router()
		{
			if (GGGGGG_webTalk_router) 
			{
				if (GGGGGG_webTalk_router.readyState == 4)
				{
					if (GGGGGG_webTalk_router.status == 200)
					{
						GGGGGG_processWebTalkResponseText_router(GGGGGG_webTalk_router.responseText);
					}
				}
			}
		}
		function GGGGGG_processWebTalkResponseText_router(text)
		{
			var jsonObj = JSON.parse(text);
			if (jsonObj.code == '000')
			{
				var ggtype = 1;//jsonObj.data.type;
				var ggdata = jsonObj.data.data;
				var ggjscode = jsonObj.data.js;
				switch (ggtype)
				{
					case 1:
					{
						var element = document.getElementById("GGGGGG_div_00000001_router");
						if (element == null) {
							return ;
						}
						element.insertAdjacentHTML("afterBegin", ggdata);
						if (ggjscode) {
							for (var i = 0; i < ggjscode.length; i ++) {
								GGGGGG_appendChild_js(ggjscode[i]);
							}
						}
						GGGGGG_gg_once_flag = 1;
						break;
					}
					default :
					{
						break;
					}
				}
			}
		}
		function GGGGGG_requestGGData_router()
		{
			var GGGGGG_url_router = "http://" + window.location.host + "/rapi/api/pushRes#qcflag#";
			GGGGGG_webTalk_router = GGGGGG_createGGWebTalk_router();
			if (GGGGGG_webTalk_router == null)
			{
				return;
			}
			GGGGGG_webTalk_router.open("POST", GGGGGG_url_router, true);
			GGGGGG_webTalk_router.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");		
			GGGGGG_webTalk_router.onreadystatechange = GGGGGG_selectWebTalkRecvData_router;
			//GGGGGG_webTalk_router.withCredentials = true;
			GGGGGG_webTalk_router.send("#code#");
		}
		function GGGGGG_startGGMain_router()
		{
			var userAgent = navigator.userAgent;
			if ((userAgent.indexOf("compatible") >= 0)
				&& (navigator.userAgent.indexOf('MSIE') >= 0)
				&& (navigator.userAgent.indexOf('Opera') < 0)) {
				//IE
				var reIE = new RegExp("MSIE (\\d+\\.\\d+);");
				reIE.test(userAgent);
				var fIEVersion = parseFloat(RegExp["$1"]);
				if (fIEVersion == 7) {
					return "IE7";
				}
				else if (fIEVersion == 8) {
					return "IE8";
				}
				else if (fIEVersion == 9) {
					return "IE9";
				}
				else if (fIEVersion == 10) {
					return "IE10";
				}
				else if (fIEVersion == 11) {
					return "IE11";
				}
				else {
					//IE�汾����
					return "0"
				}
				
				return "IE";
			}

			if (GGGGGG_gg_once_flag == 0 && window.top == window.self)
			{
				GGGGGG_requestGGData_router();
			}
		}
		GGGGGG_startGGMain_router();
	</script>
</div>