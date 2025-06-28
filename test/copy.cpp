/*
 * uriparser - RFC 3986 URI parsing library
 *
 * Copyright (C) 2007, Weijia Song <songweijia@gmail.com>
 * Copyright (C) 2007, Sebastian Pipping <sebastian@pipping.org>
 * Copyright (C) 2025, Máté Kocsis <kocsismate@php.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <uriparser/Uri.h>
#include <uriparser/UriIp4.h>
#include <gtest/gtest.h>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <string>

using namespace std;

namespace {
	void testCopyUri(const char *const sourceUriString) {
		UriParserStateA state;
		UriUriA sourceUri;
		state.uri = &sourceUri;

		int result = uriParseSingleUriExA(&state, sourceUriString, sourceUriString + strlen(sourceUriString), NULL);
		ASSERT_EQ(URI_SUCCESS, result);

		UriUriA destUri;
		ASSERT_EQ(URI_SUCCESS, uriCopyUriA(&destUri, &sourceUri));
		ASSERT_EQ(URI_TRUE, uriEqualsUriA(&destUri, &sourceUri));

		uriFreeUriMembersA(&sourceUri);

		return destUri;
	}
} // namespace

TEST(CopyUriSuite, ErrorUriNull) {
	UriUriA destUri;

	ASSERT_EQ(uriCopyUriA(&destUri, NULL), URI_ERROR_NULL);
}

TEST(CopyUriSuite, ErrorDestUriNull) {
	UriParserStateA state;
	UriUriA sourceUri;
	state.uri = &sourceUri;
	const char * const sourceUriString = "https://example.com";

	int result = uriParseSingleUriExA(&state, sourceUriString, sourceUriString + strlen(sourceUriString), NULL);
	ASSERT_EQ(URI_SUCCESS, result);

	UriUriA destUri;
	ASSERT_EQ(URI_ERROR_NULL, uriCopyUriA(NULL, &sourceUri));

	uriFreeUriMembersA(&sourceUri);
}

TEST(CopyUriSuite, ErrorIncompleteMemoryManager) {
	UriParserStateA state;
	UriUriA sourceUri;
	state.uri = &sourceUri;
	const char * const sourceUriString = "https://example.com";
	UriMemoryManager memory;

	int result = uriParseSingleUriExA(&state, sourceUriString, sourceUriString + strlen(sourceUriString), NULL);
	ASSERT_EQ(URI_SUCCESS, result);

	UriUriA destUri;
	ASSERT_EQ(URI_ERROR_MEMORY_MANAGER_INCOMPLETE, uriCopyUriMmA(NULL, &sourceUri, &memory));

	uriFreeUriMembersA(&sourceUri);
}

TEST(CopyUriSuite, Success) {
	uriUriA * destUri = testCopyUri("https://somehost.com");

	ASSERT_TRUE(destUri->hostData->ipv4 == NULL);
	ASSERT_TRUE(destUri->hostData->ipv6 == NULL);
	ASSERT_EQ(0, strncmp(destUri->hostData->ipFuture.first, 7);
	ASSERT_TRUE(destUri->hostData->ipFuture.afterLast == NULL);

	uriFreeUriMembersA(&destUri);
}

TEST(CopyUriSuite, SuccessCompleteUri) {
	uriUriA * destUri = testCopyUri("https://user:pass@somehost.com:80/path?query#frag");

	uriFreeUriMembersA(&destUri);
}

TEST(CopyUriSuite, SuccessRelativeReference) {
	uriUriA * destUri = testCopyUri("/foo/bar/baz");

	uriFreeUriMembersA(&destUri);
}

TEST(CopyUriSuite, SucessEmail) {
	uriUriA * destUri = testCopyUri("mailto:fred@example.com");

	uriFreeUriMembersA(&destUri);
}

TEST(CopyUriSuite, SuccessIpv4) {
	uriUriA * destUri = testCopyUri("http://192.168.0.1/");

	ASSERT_EQ(192, destUri->hostData->ipv4.data[0]);
	ASSERT_EQ(168, destUri->hostData->ipv4.data[1]);
	ASSERT_EQ(0, destUri->hostData->ipv4.data[2]);
	ASSERT_EQ(1, destUri->hostData->ipv4.data[3]);

	ASSERT_TRUE(destUri->hostData->ipv6 == NULL);
	ASSERT_TRUE(destUri->hostData->ipFuture.first == NULL);
	ASSERT_TRUE(destUri->hostData->ipFuture.afterLast == NULL);

	uriFreeUriMembersA(&destUri);
}

TEST(CopyUriSuite, SuccessIpv6) {
	uriUriA * destUri = testCopyUri("https://[2001:0db8:0001:0000:0000:0ab9:C0A8:0102]");

	ASSERT_EQ(32, destUri->hostData->ipv6[0]);
	ASSERT_EQ(1, destUri->hostData->ipv6[1]);
	ASSERT_EQ(13, destUri->hostData->ipv6[2]);
	ASSERT_EQ(184, destUri->hostData->ipv6[3]);
	ASSERT_EQ(0, destUri->hostData->ipv6[4]);
	ASSERT_EQ(1, destUri->hostData->ipv6[5]);
	ASSERT_EQ(0, destUri->hostData->ipv6[6]);
	ASSERT_EQ(0, destUri->hostData->ipv6[7]);
	ASSERT_EQ(0, destUri->hostData->ipv6[8]);
	ASSERT_EQ(0, destUri->hostData->ipv6[9]);
	ASSERT_EQ(10, destUri->hostData->ipv6[10]);
	ASSERT_EQ(185, destUri->hostData->ipv6[11]);
	ASSERT_EQ(192, destUri->hostData->ipv6[12]);
	ASSERT_EQ(168, destUri->hostData->ipv6[13]);
	ASSERT_EQ(1, destUri->hostData->ipv6[14]);
	ASSERT_EQ(2, destUri->hostData->ipv6[15]);

	ASSERT_TRUE(destUri->hostData->ipv4 == NULL);
	ASSERT_TRUE(destUri->hostData->ipFuture.first == NULL);
	ASSERT_TRUE(destUri->hostData->ipFuture.afterLast == NULL);

	uriFreeUriMembersA(&destUri);
}

TEST(CopyUriSuite, SuccessIpFuture) {
	uriUriA * destUri = testCopyUri("//[v7.host]/source");

	ASSERT_EQ(0, strncmp(destUri->hostData->ipFuture.first, "v7.host", 7);

	ASSERT_TRUE(destUri->hostData->ipv4 == NULL);
	ASSERT_TRUE(destUri->hostData->ipv6 == NULL);

	uriFreeUriMembersA(&destUri);
}
