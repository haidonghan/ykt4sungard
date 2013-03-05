<%
/**
 * Copyright (c) 2000-2008 Liferay, Inc. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
%>

<%@ include file="/html/portlet/message_boards/init.jsp" %>

<%
boolean editable = true;

MBTreeWalker treeWalker = (MBTreeWalker)request.getAttribute(WebKeys.MESSAGE_BOARDS_TREE_WALKER);
MBMessage selMessage = (MBMessage)request.getAttribute(WebKeys.MESSAGE_BOARDS_TREE_WALKER_SEL_MESSAGE);
MBMessage message = (MBMessage)request.getAttribute(WebKeys.MESSAGE_BOARDS_TREE_WALKER_CUR_MESSAGE);
MBCategory category = (MBCategory)request.getAttribute(WebKeys.MESSAGE_BOARDS_TREE_WALKER_CATEGORY);
boolean lastNode = ((Boolean)request.getAttribute(WebKeys.MESSAGE_BOARDS_TREE_WALKER_LAST_NODE)).booleanValue();
int depth = ((Integer)request.getAttribute(WebKeys.MESSAGE_BOARDS_TREE_WALKER_DEPTH)).intValue();

String className = "portlet-section-alternate";
String classHoverName = "portlet-section-alternate-hover";

if (treeWalker.isOdd()) {
	className = "portlet-section-body";
	classHoverName = "portlet-section-body-hover";
}
%>

<tr class="<%= className %>" onMouseEnter="this.className = '<%= classHoverName %>';" onMouseLeave="this.className = '<%= className %>';">
	<td style="padding-left: <%= depth > 0 ? depth * 10 : 5  %>px;" valign="middle" width="90%">
		<c:if test="<%= !message.isRoot() %>">
			<c:choose>
				<c:when test="<%= !lastNode %>">
					<img src="<%= themeDisplay.getPathThemeImages() %>/message_boards/t.png" />
				</c:when>
				<c:otherwise>
					<img src="<%= themeDisplay.getPathThemeImages() %>/message_boards/l.png" />
				</c:otherwise>
			</c:choose>
		</c:if>

		<%
		String portalURL = PortalUtil.getPortalURL(themeDisplay);
		String layoutURL = PortalUtil.getLayoutURL(themeDisplay);

		String messageURL = portalURL + layoutURL + "/message_boards/message/" + selMessage.getMessageId();

		String rowHREF = messageURL + "#" + renderResponse.getNamespace() + "message_" + message.getMessageId();
		%>

		<a href="<%= rowHREF %>">

		<%
		boolean readFlag = true;

		if (themeDisplay.isSignedIn()) {
			readFlag = MBMessageFlagLocalServiceUtil.hasReadFlag(themeDisplay.getUserId(), message.getMessageId());
		}
		%>

		<c:if test="<%= !readFlag %>">
			<b>
		</c:if>

		<%= Html.escape(message.getSubject()) %>

		<c:if test="<%= !readFlag %>">
			</b>
		</c:if>

		</a>
	</td>
	<td nowrap>
		<a href="<%= rowHREF %>">

		<c:if test="<%= !readFlag %>">
			<b>
		</c:if>

		<c:choose>
			<c:when test="<%= message.isAnonymous() %>">
				<liferay-ui:message key="anonymous" />
			</c:when>
			<c:otherwise>
				<%= PortalUtil.getUserName(message.getUserId(), message.getUserName()) %>
			</c:otherwise>
		</c:choose>

		<c:if test="<%= !readFlag %>">
			</b>
		</c:if>

		</a>
	</td>
	<td nowrap>
		<a href="<%= rowHREF %>">
		<%= dateFormatDateTime.format(message.getModifiedDate()) %>
		</a>
	</td>
</tr>

<%
List messages = treeWalker.getMessages();
int[] range = treeWalker.getChildrenRange(message);

depth++;

for (int i = range[0]; i < range[1]; i++) {
	MBMessage curMessage = (MBMessage)messages.get(i);

	boolean lastChildNode = false;

	if ((i + 1) == range[1]) {
		lastChildNode = true;
	}

	request.setAttribute(WebKeys.MESSAGE_BOARDS_TREE_WALKER, treeWalker);
	request.setAttribute(WebKeys.MESSAGE_BOARDS_TREE_WALKER_SEL_MESSAGE, selMessage);
	request.setAttribute(WebKeys.MESSAGE_BOARDS_TREE_WALKER_CUR_MESSAGE, curMessage);
	request.setAttribute(WebKeys.MESSAGE_BOARDS_TREE_WALKER_CATEGORY, category);
	request.setAttribute(WebKeys.MESSAGE_BOARDS_TREE_WALKER_LAST_NODE, Boolean.valueOf(lastChildNode));
	request.setAttribute(WebKeys.MESSAGE_BOARDS_TREE_WALKER_DEPTH, new Integer(depth));
%>

	<liferay-util:include page="/html/portlet/message_boards/view_thread_shortcut.jsp" />

<%
}
%>

<%!
private static Log _log = LogFactoryUtil.getLog("portal-web.docroot.html.portlet.message_boards.view_thread_shortcut.jsp");
%>