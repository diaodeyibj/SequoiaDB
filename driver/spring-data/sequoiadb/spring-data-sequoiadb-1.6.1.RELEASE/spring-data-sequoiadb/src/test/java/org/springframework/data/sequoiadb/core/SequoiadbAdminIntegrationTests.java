/*
 * Copyright 2002-2011 the original author or authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.springframework.data.sequoiadb.core;

import org.springframework.data.sequoiadb.assist.CommandResult;
import org.springframework.data.sequoiadb.assist.DB;
import org.springframework.data.sequoiadb.assist.Sdb;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit4.SpringJUnit4ClassRunner;

/**
 * This test class assumes that you are already running the SequoiaDB server.
 * 

 */
@RunWith(SpringJUnit4ClassRunner.class)
@ContextConfiguration("classpath:infrastructure.xml")
public class SequoiadbAdminIntegrationTests {

	private static Log logger = LogFactory.getLog(SequoiadbAdminIntegrationTests.class);

	@SuppressWarnings("unused")
	private DB testAdminDb;

	@Autowired
	Sdb sdb;

	@Before
	public void setUp() {
		sdb.getDB("testAdminDb").dropDatabase();
		testAdminDb = sdb.getDB("testAdminDb");

	}

	@Test
	public void serverStats() {
		// CommandResult result = testAdminDb.getStats();
		CommandResult result = sdb.getDB("admin").command("serverStatus");
		logger.info("stats = " + result);
	}
}