/*
 * Copyright 2011-2014 the original author or authors.
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
package org.springframework.data.sequoiadb.repository.query;

import static org.hamcrest.Matchers.*;
import static org.junit.Assert.*;

import java.lang.reflect.Method;
import java.util.Collection;
import java.util.List;

import org.junit.Before;
import org.junit.Test;
import org.springframework.data.domain.Pageable;
import org.springframework.data.geo.Distance;
import org.springframework.data.geo.GeoPage;
import org.springframework.data.geo.GeoResult;
import org.springframework.data.geo.GeoResults;
import org.springframework.data.geo.Point;
import org.springframework.data.sequoiadb.core.User;
import org.springframework.data.sequoiadb.core.mapping.SequoiadbMappingContext;
import org.springframework.data.sequoiadb.repository.Address;
import org.springframework.data.sequoiadb.repository.Contact;
import org.springframework.data.sequoiadb.repository.Meta;
import org.springframework.data.sequoiadb.repository.Person;
import org.springframework.data.repository.Repository;
import org.springframework.data.repository.core.support.DefaultRepositoryMetadata;

/**
 * Unit test for {@link SequoiadbQueryMethod}.
 * 


 */
public class SdbQueryMethodUnitTests {

	SequoiadbMappingContext context;

	@Before
	public void setUp() {
		context = new SequoiadbMappingContext();
	}

	@Test
	public void detectsCollectionFromRepoTypeIfReturnTypeNotAssignable() throws Exception {

		Method method = SampleRepository.class.getMethod("method");

		SequoiadbQueryMethod queryMethod = new SequoiadbQueryMethod(method, new DefaultRepositoryMetadata(SampleRepository.class),
				context);
		SequoiadbEntityMetadata<?> metadata = queryMethod.getEntityInformation();

		assertThat(metadata.getJavaType(), is(typeCompatibleWith(Address.class)));
		assertThat(metadata.getCollectionName(), is("contact"));
	}

	@Test
	public void detectsCollectionFromReturnTypeIfReturnTypeAssignable() throws Exception {

		Method method = SampleRepository2.class.getMethod("method");

		SequoiadbQueryMethod queryMethod = new SequoiadbQueryMethod(method, new DefaultRepositoryMetadata(SampleRepository.class),
				context);
		SequoiadbEntityMetadata<?> entityInformation = queryMethod.getEntityInformation();

		assertThat(entityInformation.getJavaType(), is(typeCompatibleWith(Person.class)));
		assertThat(entityInformation.getCollectionName(), is("person"));
	}

	@Test
	public void discoversUserAsDomainTypeForGeoPageQueryMethod() throws Exception {

		SequoiadbQueryMethod queryMethod = queryMethod("findByLocationNear", Point.class, Distance.class, Pageable.class);
		assertThat(queryMethod.isGeoNearQuery(), is(true));
		assertThat(queryMethod.isPageQuery(), is(true));

		queryMethod = queryMethod("findByFirstname", String.class, Point.class);
		assertThat(queryMethod.isGeoNearQuery(), is(true));
		assertThat(queryMethod.isPageQuery(), is(false));
		assertThat(queryMethod.getEntityInformation().getJavaType(), is(typeCompatibleWith(User.class)));

		assertThat(queryMethod("findByEmailAddress", String.class, Point.class).isGeoNearQuery(), is(true));
		assertThat(queryMethod("findByFirstname", String.class, Point.class).isGeoNearQuery(), is(true));
		assertThat(queryMethod("findByLastname", String.class, Point.class).isGeoNearQuery(), is(true));
	}

	@Test(expected = IllegalArgumentException.class)
	public void rejectsGeoPageQueryWithoutPageable() throws Exception {
		queryMethod("findByLocationNear", Point.class, Distance.class);
	}

	@Test(expected = IllegalArgumentException.class)
	public void rejectsNullMappingContext() throws Exception {
		Method method = PersonRepository.class.getMethod("findByFirstname", String.class, Point.class);
		new SequoiadbQueryMethod(method, new DefaultRepositoryMetadata(PersonRepository.class), null);
	}

	@Test
	public void considersMethodReturningGeoPageAsPagingMethod() throws Exception {
		SequoiadbQueryMethod method = queryMethod("findByLocationNear", Point.class, Distance.class, Pageable.class);
		assertThat(method.isPageQuery(), is(true));
		assertThat(method.isCollectionQuery(), is(false));
	}

	@Test
	public void createsSequoiadbQueryMethodObjectForMethodReturningAnInterface() throws Exception {

		Method method = SampleRepository2.class.getMethod("methodReturningAnInterface");
		new SequoiadbQueryMethod(method, new DefaultRepositoryMetadata(SampleRepository2.class), context);
	}

	/**
	 * @see DATA_JIRA-957
	 */
	@Test
	public void createsSequoiadbQueryMethodWithEmptyMetaCorrectly() throws Exception {

		SequoiadbQueryMethod method = queryMethod("emptyMetaAnnotation");
		assertThat(method.hasQueryMetaAttributes(), is(true));
		assertThat(method.getQueryMetaAttributes().hasValues(), is(false));
	}

	/**
	 * @see DATA_JIRA-957
	 */
	@Test
	public void createsSequoiadbQueryMethodWithMaxExecutionTimeCorrectly() throws Exception {

		SequoiadbQueryMethod method = queryMethod("metaWithMaxExecutionTime");
		assertThat(method.hasQueryMetaAttributes(), is(true));
		assertThat(method.getQueryMetaAttributes().getMaxTimeMsec(), is(100L));
	}

	/**
	 * @see DATA_JIRA-957
	 */
	@Test
	public void createsSequoiadbQueryMethodWithMaxScanCorrectly() throws Exception {

		SequoiadbQueryMethod method = queryMethod("metaWithMaxScan");
		assertThat(method.hasQueryMetaAttributes(), is(true));
		assertThat(method.getQueryMetaAttributes().getMaxScan(), is(10L));
	}

	/**
	 * @see DATA_JIRA-957
	 */
	@Test
	public void createsSequoiadbQueryMethodWithCommentCorrectly() throws Exception {

		SequoiadbQueryMethod method = queryMethod("metaWithComment");
		assertThat(method.hasQueryMetaAttributes(), is(true));
		assertThat(method.getQueryMetaAttributes().getComment(), is("foo bar"));
	}

	/**
	 * @see DATA_JIRA-957
	 */
	@Test
	public void createsSequoiadbQueryMethodWithSnapshotCorrectly() throws Exception {

		SequoiadbQueryMethod method = queryMethod("metaWithSnapshotUsage");
		assertThat(method.hasQueryMetaAttributes(), is(true));
		assertThat(method.getQueryMetaAttributes().getSnapshot(), is(true));
	}

	private SequoiadbQueryMethod queryMethod(String name, Class<?>... parameters) throws Exception {
		Method method = PersonRepository.class.getMethod(name, parameters);
		return new SequoiadbQueryMethod(method, new DefaultRepositoryMetadata(PersonRepository.class), context);
	}

	interface PersonRepository extends Repository<User, Long> {

		// Misses Pageable
		GeoPage<User> findByLocationNear(Point point, Distance distance);

		GeoPage<User> findByLocationNear(Point point, Distance distance, Pageable pageable);

		GeoResult<User> findByEmailAddress(String lastname, Point location);

		GeoResults<User> findByFirstname(String firstname, Point location);

		Collection<GeoResult<User>> findByLastname(String lastname, Point location);

		@Meta
		List<User> emptyMetaAnnotation();

		@Meta(maxExcecutionTime = 100)
		List<User> metaWithMaxExecutionTime();

		@Meta(maxScanDocuments = 10)
		List<User> metaWithMaxScan();

		@Meta(comment = "foo bar")
		List<User> metaWithComment();

		@Meta(snapshot = true)
		List<User> metaWithSnapshotUsage();

	}

	interface SampleRepository extends Repository<Contact, Long> {

		List<Address> method();
	}

	interface SampleRepository2 extends Repository<Contact, Long> {

		List<Person> method();

		Customer methodReturningAnInterface();
	}

	interface Customer {

	}

}
