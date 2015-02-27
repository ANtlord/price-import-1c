--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: service_category; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE service_category (
    id integer NOT NULL,
    name character varying(255) NOT NULL,
    sort integer NOT NULL,
    short_text character varying(255) DEFAULT ''::character varying NOT NULL,
    image character varying(255),
    background character varying(7),
    is_active boolean NOT NULL,
    letter_image character varying(255),
    section_id integer NOT NULL
);


ALTER TABLE service_category OWNER TO postgres;

--
-- Name: service_category_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE service_category_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE service_category_id_seq OWNER TO postgres;

--
-- Name: service_category_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE service_category_id_seq OWNED BY service_category.id;


--
-- Name: service_company; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE service_company (
    id integer NOT NULL,
    text text,
    sort integer NOT NULL,
    name character varying(255) DEFAULT 'qwe'::character varying NOT NULL,
    image character varying(255),
    short_text character varying(255) DEFAULT ''::character varying NOT NULL,
    created_at timestamp with time zone NOT NULL,
    background character varying(7),
    is_active boolean NOT NULL,
    address character varying(255),
    letter_image character varying(255),
    section_id integer
);


ALTER TABLE service_company OWNER TO postgres;

--
-- Name: service_company_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE service_company_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE service_company_id_seq OWNER TO postgres;

--
-- Name: service_company_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE service_company_id_seq OWNED BY service_company.id;


--
-- Name: service_industry; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE service_industry (
    id integer NOT NULL,
    name character varying(255) NOT NULL,
    sort integer NOT NULL,
    short_text character varying(255) DEFAULT ''::character varying NOT NULL,
    image character varying(255),
    background character varying(7),
    is_active boolean NOT NULL,
    letter_image character varying(255)
);


ALTER TABLE service_industry OWNER TO postgres;

--
-- Name: service_industry_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE service_industry_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE service_industry_id_seq OWNER TO postgres;

--
-- Name: service_industry_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE service_industry_id_seq OWNED BY service_industry.id;


--
-- Name: service_product; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE service_product (
    id integer NOT NULL,
    name character varying(255) NOT NULL,
    sort integer NOT NULL,
    text text,
    price numeric(10,2) NOT NULL,
    short_text character varying(255) DEFAULT ''::character varying NOT NULL,
    image character varying(255),
    background character varying(7),
    is_active boolean NOT NULL,
    letter_image character varying(255),
    section_id integer NOT NULL,
    code character varying(255)
);


ALTER TABLE service_product OWNER TO postgres;

--
-- Name: service_product_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE service_product_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE service_product_id_seq OWNER TO postgres;

--
-- Name: service_product_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE service_product_id_seq OWNED BY service_product.id;


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY service_category ALTER COLUMN id SET DEFAULT nextval('service_category_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY service_company ALTER COLUMN id SET DEFAULT nextval('service_company_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY service_industry ALTER COLUMN id SET DEFAULT nextval('service_industry_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY service_product ALTER COLUMN id SET DEFAULT nextval('service_product_id_seq'::regclass);


--
-- Name: service_category_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY service_category
    ADD CONSTRAINT service_category_pkey PRIMARY KEY (id);


--
-- Name: service_category_section_id_12ee35c83bc400ea_uniq; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY service_category
    ADD CONSTRAINT service_category_section_id_12ee35c83bc400ea_uniq UNIQUE (section_id, name);


--
-- Name: service_company_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY service_company
    ADD CONSTRAINT service_company_pkey PRIMARY KEY (id);


--
-- Name: service_industry_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY service_industry
    ADD CONSTRAINT service_industry_pkey PRIMARY KEY (id);


--
-- Name: service_product_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY service_product
    ADD CONSTRAINT service_product_pkey PRIMARY KEY (id);


--
-- Name: service_category_section_id; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX service_category_section_id ON service_category USING btree (section_id);


--
-- Name: service_company_section_id; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX service_company_section_id ON service_company USING btree (section_id);


--
-- Name: service_product_section_id; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX service_product_section_id ON service_product USING btree (section_id);


--
-- Name: section_id_refs_id_222ba1b9; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY service_category
    ADD CONSTRAINT section_id_refs_id_222ba1b9 FOREIGN KEY (section_id) REFERENCES service_company(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: section_id_refs_id_dae8f641; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY service_product
    ADD CONSTRAINT section_id_refs_id_dae8f641 FOREIGN KEY (section_id) REFERENCES service_category(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: section_id_refs_id_ed7ac773; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY service_company
    ADD CONSTRAINT section_id_refs_id_ed7ac773 FOREIGN KEY (section_id) REFERENCES service_industry(id) DEFERRABLE INITIALLY DEFERRED;


--
-- PostgreSQL database dump complete
--

